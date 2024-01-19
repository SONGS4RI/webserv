#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

void exit_with_perror(const string& msg) {
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

void change_events(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
					uint16_t flags, uint32_t fflags, intptr_t data, void* udata) {
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(tmp_event);
}

void disconnect_client(int client_fd, map<int, string>& clients) {
	cout << "client disconnected: " << client_fd << endl;
	close(client_fd);
	clients.erase(client_fd);
}

int main() {
	/* 서버 소켓 초기화 & listen */
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;

	if (server_socket == -1) {
		exit_with_perror("socket() eroror: " + string(strerror(errno)));
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// 모든 ip접근 허가
	server_addr.sin_port = htons(8080);// 포트 설정

	if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1 ) {
		exit_with_perror("bind() eroror: " + string(strerror(errno)));
	}

	if (listen(server_socket, 5) == -1) {
		exit_with_perror("listen() eroror: " + string(strerror(errno)));
	}
	fcntl(server_socket, F_SETFL, O_NONBLOCK);

	/* kqueue 초기화*/
	int kq = kqueue();
	if (kq == -1) {
		exit_with_perror("kq() eroror: " + string(strerror(errno)));
	}

	map<int, string> clients;// 클라이언트 소켓 데이터
	vector<struct kevent> change_list;// kevent 배열
	struct kevent event_list[8];// event list 배열

	/* 서버 소켓에 이벤트 추가 */
	change_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	cout << "echo server started" << endl;

	/* 메인 루프 */
	int new_events;
	struct kevent* cur_event;
	while (1) {
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
		if (new_events == -1) {
			exit_with_perror("kevent() eroror: " + string(strerror(errno)));
		}
		change_list.clear();
		for (int i=0; i<new_events; i++) {
			cur_event = &event_list[i];

			/* 에러 이벤트 체크 */
			if (cur_event->flags & EV_ERROR) {
				if (cur_event->ident == server_socket) {
					exit_with_perror("server socket eroror: " + string(strerror(errno)));
				} else {
					cerr << "client socket error" << endl;
					disconnect_client(cur_event->ident, clients);
				}
			} else if (cur_event->filter == EVFILT_READ) {
				if (cur_event->ident == server_socket) {
					/* 새로운 클라이언트 승인 */
					int client_socket = accept(server_socket, NULL, NULL);
					if (client_socket == -1) {
						exit_with_perror("accept() eroror: " + string(strerror(errno)));
					}
					cout << "accept new client: " << client_socket << endl;
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					/* 클라 소켓 이벤트 추가 - 쓰기 읽기 이벤트 추가 */
					change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[client_socket] = "";
				} else if (clients.find(cur_event->ident) != clients.end()) {

				}
			}
		}
	}

}