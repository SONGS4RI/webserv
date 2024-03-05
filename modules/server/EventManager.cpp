#include <sys/event.h>
#include "EventManager.hpp"
#include "SocketManager.hpp"
#include "../response/Response.hpp"
#include "../request_handler/RequestHandler.hpp"
#include "../utils/StatusCode.hpp"
EventManager* EventManager::ev = NULL;

EventManager::EventManager() {
	init();
}

EventManager::~EventManager() {
	close(kq);
}

void EventManager::init() {
	kq = kqueue();
	if (kq == -1) {
		Utils::exitWithErrmsg("Error: kqueue()");
	}
	change_list.reserve(EVENT_MAX_CNT);
}

EventManager* EventManager::getInstance() {
	if (!ev) {
		try {
			ev = new EventManager();
		} catch (exception& e) {
			Utils::exitWithErrmsg(e.what());
		}
	}
	return (ev);
}

int EventManager::detectEvent() {
	Utils::log(Utils::intToString(change_list.size()) + "....", CYAN);
	int newEvents = kevent(kq, &change_list[0], change_list.size(), event_list, EVENT_MAX_CNT, NULL);
	Utils::log(Utils::intToString(change_list.size() + 1) + "....", CYAN);
	if (newEvents < 0) {
		Utils::exitWithErrmsg("Detect Event Error");
	}
	change_list.clear();
	return (newEvents);
}

void EventManager::addEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags,
							intptr_t data, void* udata) {
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(tmp_event);
}

void EventManager::changeEvent(struct kevent* curEvent, int16_t filter, uint16_t flags,
								uint32_t fflags, intptr_t data, void* udata) {
	EV_SET(curEvent, curEvent->ident, filter, flags, fflags, data, udata);
}

void EventManager::handleEvent(const int& eventIdx) {
	struct kevent* curEvent = &event_list[eventIdx];
	SocketManager* sm = SocketManager::getInstance();
	Client* client = sm->getClient(curEvent->ident);
	if (curEvent->flags & EV_ERROR) {// 에러
		if (sm->isServerSocket(curEvent->ident)) {
			Utils::exitWithErrmsg("Server Socket Error");
		}
		Utils::log("client socket error", RED);
		sm->disconnectClient(curEvent->ident);
	} else if (curEvent->filter ==  EVFILT_READ) {// 읽기
		if (sm->isServerSocket(curEvent->ident)) {
			// 새로운 클라이언트 등록 및 read 이벤트 생성
			int clientSocket;
			try {
				clientSocket = sm->acceptClient(curEvent->ident);
				if (clientSocket == -1) { //accept()를 실패해서 클라이언트 소켓 할당도 안된 경우
					Utils::log("accept() Failed", YELLOW);
					return ;
				}
				sm->addClient(curEvent->ident, clientSocket);//throw errCode 발생 가능
				addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			} catch (StatusCode& errCode) {
				//accept는 성공했는데 다른 문제가 발생한 경우 -> 즉시 에러 리스폰스 만든다.
				Utils::log("accept() Successed But Something went wrong", YELLOW);
				Response*	errResponse = new Response(errCode);
				addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, errResponse);
			}
		} else if (curEvent->udata == NULL) {
			// 클라이언트의 처리 상태에 따라 이벤트 처리
			Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": Parsing", GREEN);
			Request* request = client->getRequest(curEvent->ident);// NULL 이면 할당해서 주기
			request->parseRequest(*client);
			if (request->getStatus() == PARSE_DONE || request->getStatusCode().getStatusCode() >= 400) {
				Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": " +
				(request->getStatus() == PARSE_DONE ? "Parse Done" : "Parse Error: " + request->getStatusCode().getMessage()), GREEN);
				changeEvent(curEvent, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, request);///////////////////////////여기
			}
		}
	} else if (curEvent->filter ==  EVFILT_WRITE) {// 쓰기
		if (curEvent->udata != NULL) {// udata -> Request
			Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": Handle Request Start", GREEN);
			Request* request = (Request *)curEvent->udata;
			RequestHandler requestHandler = RequestHandler(request, client);
			ResponseBody* responseBody = requestHandler.handleRequest();
			if (responseBody != NULL) {// cgi 아니라면
				Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": Response Created: " +
				responseBody->getStatusCode().getMessage(), GREEN);
				Response* response = new Response(responseBody);
				client->setResponse(response);
				changeEvent(curEvent, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, response);
			}
		}
		if (curEvent->udata != NULL) {/// udata -> response
			Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": Writing", GREEN);
			Response* response = (Response *)curEvent->udata;
			response->writeToSocket(curEvent->ident);
			if (response->isDone()) {
				Utils::log("Client: " + Utils::intToString(curEvent->ident) + ": Write Done", GREEN);
				sm->disconnectClient(curEvent->ident);
				changeEvent(curEvent, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			}
		}
	}
}