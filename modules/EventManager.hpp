#pragma once

#include <sys/types.h>
#include <sys/event.h>

#include <vector>
#include <iostream>

using namespace std;

class EventManager {
	private:
		static EventManager* ev;
		int kq;
		vector<struct kevent> change_list;// kevent 배열
		struct kevent event_list[8];// event list 배열
		EventManager();
	public:
		static EventManager* getInstance();// 싱들톤

		~EventManager();// 동적할당 해제해야함
		void init();// kqueue()
		void changeEvents(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);// EV_SET 사용
		int detectEvent();
		void handleEvent(const int& eventIdx);// write, read

};
