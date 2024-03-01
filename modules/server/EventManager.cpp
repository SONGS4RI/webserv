#include "EventManager.hpp"
#include "SocketManager.hpp"
#include "../RequestHandler.hpp"
#include "../utils/StatusCode.hpp"
static EventManager* ev = NULL;

EventManager::EventManager() {
	init();
}

EventManager* EventManager::getInstance() {
	if (!ev) {
		ev = new EventManager();
	}
	return (ev);
}

int EventManager::detectEvent() {
	kevent(kq, &change_list[0], change_list.size(), event_list, EVENT_MAX_CNT, NULL);
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

	if (curEvent->flags & EV_ERROR) {// 에러
		if (sm->isServerSocket(curEvent->ident)) {
			// 서버 에러 발생하면 안되지만 일단 체크
			exit(1);
		}
		cerr << "client socket error" << endl;
		sm->disconnectClient(curEvent->ident);
	} else if (curEvent->flags ==  EVFILT_READ) {// 읽기
		Client client = sm->getClient(curEvent->ident);
		if (sm->isServerSocket(curEvent->ident)) {
			// 새로운 클라이언트 등록 및 read 이벤트 생성
			// sm->clients.push_back(Client());
			int clientSocket = sm->acceptClient(curEvent->ident);
			addEvent(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		} else if (curEvent->udata == NULL) {
			// 클라이언트의 처리 상태에 따라 이벤트 처리
			Request* request = client.getCurReqeust();// NULL 이면 할당해서 주기
			request->parseRequest(client);
			if (request->getStatus() == PARSE_DONE || request->getStatusCode().getStatusCode() >= 400) {
				changeEvent(curEvent, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, request);
			}
		} else {
			Request* request = (Request *)curEvent->udata;
			RequestHandler requestHandler = RequestHandler(request);
			ResponseBody* responseBody = requestHandler.handleRequest();
			// client->setResponse(responseBody);
			// changeEvent(curEvent, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, client->getResponse());
		}
	} else if (curEvent->flags ==  EVFILT_WRITE) {// 쓰기
		/*
		뭔가 처리한 결과 데이터 보내기
		*/
	}
}