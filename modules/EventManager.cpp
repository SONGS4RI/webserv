#include "EventManager.hpp"
#include "SocketManager.hpp"

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
	kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
}


void EventManager::handleEvent(const int& eventIdx) {
	struct kevent* cur_event = &event_list[eventIdx];
	SocketManager* sm = SocketManager::getInstance();

	if (cur_event->flags & EV_ERROR) {// 에러
		if (sm->isServerSocket(cur_event->ident)) {
			// 서버 에러 발생하면 안되지만 일단 체크
			exit(1);
		}
		cerr << "client socket error" << endl;
		sm->disconnect_client(cur_event->ident);
	} else if (cur_event->flags ==  EVFILT_READ) {// 읽기
		if (sm->isServerSocket(cur_event->ident)) {
			// sm->clients.push_back(Client());
		} else /* 이미 연결된 클라이언트 */ {
			Client client = sm->getClient(cur_event->ident);
			// 클라이언트의 처리 상태에 따라 이벤트 처리
			Request* request = client.getCurReqeust();
			/**
			 * 1. http 메세지를 다 읽고 파싱했는데 이후 request 까지 읽어서 leftover가 남은 경우 0 -> PARSE_DONE에서 처리
			 * 2. http 메세지를 다 읽고 파싱해 request가 없어진 상태
			 * 3. http 메세지를 다 못 읽은 경우 null 일리 없음
			*/
			request->parseRequest(client);
		}
	} else if (cur_event->flags ==  EVFILT_WRITE) {// 쓰기
		/*
		뭔가 처리한 결과 데이터 보내기
		*/
	}
}