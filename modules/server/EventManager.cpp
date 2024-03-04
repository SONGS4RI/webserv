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
		exitWithErrmsg("Error: kqueue()");
	}
}

EventManager* EventManager::getInstance() {
	if (!ev) {
		try {
			ev = new EventManager();
		} catch (exception& e) {
			exitWithErrmsg(e.what());
		}
	}
	return (ev);
}

int EventManager::detectEvent() {
	return (kevent(kq, &change_list[0], change_list.size(), event_list, EVENT_MAX_CNT, NULL));
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

	if (curEvent->filter & EV_ERROR) {// 에러
		if (sm->isServerSocket(curEvent->ident)) {
			// 서버 에러 발생하면 안되지만 일단 체크
			exit(1);
		}
		cerr << "client socket error" << endl;
		sm->disconnectClient(curEvent->ident);
	} else if (curEvent->filter ==  EVFILT_READ) {// 읽기
		Client* client = sm->getClient(curEvent->ident);
		if (sm->isServerSocket(curEvent->ident)) {
			// 새로운 클라이언트 등록 및 read 이벤트 생성
			int clientSocket;
			try {
				clientSocket = sm->acceptClient(curEvent->ident);
				if (clientSocket == -1) { //accept()를 실패해서 클라이언트 소켓 할당도 안된 경우
					return ;
				}
				sm->addClient(curEvent->ident, clientSocket);//throw errCode 발생 가능
				addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			} catch (StatusCode& errCode) {
				//accept는 성공했는데 다른 문제가 발생한 경우 -> 즉시 에러 리스폰스 만든다.
				Response*	errResponse = new Response(errCode);
				addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, errResponse);
			}
		} else if (curEvent->udata == NULL) {
			// 클라이언트의 처리 상태에 따라 이벤트 처리
			Request* request = client->getReqeust();// NULL 이면 할당해서 주기
			request->parseRequest(*client);
			if (request->getStatus() == PARSE_DONE || request->getStatusCode().getStatusCode() >= 400) {
				changeEvent(curEvent, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, request);
			}
		} else {
			Request* request = (Request *)curEvent->udata;
			RequestHandler requestHandler = RequestHandler(request, client);
			ResponseBody* responseBody = requestHandler.handleRequest();
			if (responseBody != NULL) {// cgi 아니라면
				Response* response = new Response(responseBody);
				client->setResponse(response);
				changeEvent(curEvent, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, response);
			}
		}
	} else if (curEvent->filter ==  EVFILT_WRITE) {// 쓰기
		if (curEvent->udata != NULL) {//쓸게 있으면 buffsize만큼 쓰기
			Response* response = (Response *)curEvent->udata;
			response->writeToSocket(curEvent->ident);
			if (response->isDone()) {
				sm->disconnectClient(curEvent->ident);
			}
		}
	}
}