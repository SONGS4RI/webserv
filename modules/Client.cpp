#include <fcntl.h>
#include "Client.hpp"

Client::Client(int _clientSocket) : request(NULL), response(NULL), clientSocket(_clientSocket) {
	if (fcntl(_clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		/* 에러 처리 */
	}
}

Client::~Client() {
	close(clientSocket);
}