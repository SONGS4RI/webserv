#include <fcntl.h>
#include "Client.hpp"

Client::Client(int _clientSocket) : request(NULL), response(NULL), clientSocket(_clientSocket) {
	fcntl(_clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}