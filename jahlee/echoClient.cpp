#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>

#define BUF_SIZE 1024
using namespace std;

void exit_with_perror(const string& msg) {
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	if (argc != 3) {
		exit_with_perror(string(argv[0]) + " [IP] [PORT]");
	}
	/* 클라이언트 소켓 초기화 & connect */
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);// 클라이언트 소켓 IPv4, TCP, 0
	struct sockaddr_in server_addr;

	if (client_socket == -1) {
		exit_with_perror("socket() eroror: " + string(strerror(errno)));
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;// TCP, UDP 설정
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);// IP
	server_addr.sin_port = htons(atoi(argv[2]));// PORT

	if (connect(client_socket, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		exit_with_perror("connect() eroror: " + string(strerror(errno)));
	}

	char message[BUF_SIZE];
	int len;
	while (1) {
		/* 메세지 송신 */
		cout << "전송할 메세지를 입력: " << endl;
		cin.getline(message, BUF_SIZE);
		write(client_socket, message, strlen(message));
		/* 메세지 수신 */
		len = read(client_socket, message, BUF_SIZE - 1);
		message[len] = '\0';
		cout << "수신한 메세지: " << message << endl;
	}

	close(client_socket);
	return 0;
}