#include <fcntl.h>
#include "Client.hpp"

Client::Client(int _clientSocket, const Server& _server) :
myServer(_server), request(NULL), response(NULL), clientSocket(_clientSocket) {
	if (fcntl(_clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		/* 에러 처리!!! */
	}
}

Client::~Client() {
	close(clientSocket);
}

const Server&	Client::getServer() const { return(myServer);}

Request* Client::getReqeust() { return (request);}

Response* Client::getResponse() { return (response);}

const int& Client::getSocketFd() { return (clientSocket);}

void Client::addRequest(Request* _request) { request = _request;}

void Client::addResponse(Response* _response) { response = _response;}