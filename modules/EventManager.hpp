#pragma once

#include <sys/types.h>
#include <sys/event.h>

#include <vector>
#include <iostream>

#define EVENT_MAX_CNT 128

using namespace std;

class EventManager {
	private:
		static EventManager* ev;
		int kq;
		vector<struct kevent> change_list;// kevent 배열
		struct kevent event_list[EVENT_MAX_CNT];// event list 배열
		EventManager();
	public:
		static EventManager* getInstance();// 싱들톤

		~EventManager();
		void init();// kqueue()
		void changeEvents(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata);// EV_SET 사용
		int detectEvent();
		void handleEvent(const int& eventIdx);// write, read

};
