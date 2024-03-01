#include <fcntl.h>
#include "Client.hpp"

Client::Client(int _clientSocket, const Server& _server) :
myServer(_server), request(NULL), response(NULL), clientSocket(_clientSocket) {
	if (fcntl(_clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		/* 에러 처리!!! */
	}
}

Client::~Client() {
	if (request != NULL) {
		delete request;
	}
	if (response != NULL) {
		delete request;
	}
	close(clientSocket);
}

void Client::setRequest(Request* _request) { request = _request;}

void Client::setResponse(Response* _response) { response = _response;}

const Server&	Client::getServer() const { return(myServer);}

Request* Client::getReqeust() { return (request);}

Response* Client::getResponse() { return (response);}

const int& Client::getSocketFd() { return (clientSocket);}
