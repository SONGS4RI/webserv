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
#include <cstdio>

using namespace std;

void exit_with_perror(const string& msg) {
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		exit_with_perror(string(argv[0]) + " [PORT]");
	}
	/* 서버 소켓 초기화 & listen */
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);// 서버 소켓 IPv4, TCP, 0
	struct sockaddr_in server_addr;

	if (server_socket == -1) {
		exit_with_perror("socket() eroror: " + string(strerror(errno)));
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;// TCP, UDP 설정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// 모든 ip접근 허가
	server_addr.sin_port = htons(atoi(argv[1]));// 포트 설정

	if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1 ) {
		exit_with_perror("bind() eroror: " + string(strerror(errno)));
	}

	if (listen(server_socket, 5) == -1) {
		exit_with_perror("listen() eroror: " + string(strerror(errno)));
	}
	// fcntl(server_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);// 소켓 플레그를 논블로킹으로 설정

	cout << "echo server started" << endl;

	int client_socket = accept(server_socket, NULL, NULL);
	if (client_socket == -1) {
		exit_with_perror("accept() eroror: " + string(strerror(errno)));
	}

	char buf[1024];
	int index_fd = open("index.html", O_RDONLY);
	int n = read(index_fd, buf, sizeof(buf));

	string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: " + to_string(n) + "\r\n\r\n" + buf;
	write(client_socket, response_header.c_str(), response_header.size());
	
	close(client_socket);
	remove("test.txt");
	return 0;
}