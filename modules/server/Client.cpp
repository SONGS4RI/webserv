#include "Client.hpp"

Client::Client(int _clientSocket, const Server& _server) :
myServer(_server), request(NULL), response(NULL), clientSocket(_clientSocket), pipeFd(-1), pid(-1) {}

Client::~Client() {
	if (request != NULL) {
		delete request;
	}
	if (response != NULL) {
		delete response;
	}
	close(clientSocket);
}

void Client::setRequest(Request* _request) { request = _request;}

void Client::setResponse(Response* _response) { response = _response;}

const Server&	Client::getServer() const { return(myServer);}

Request* Client::getReqeust() { return (request);}

Response* Client::getResponse() { return (response);}

const int& Client::getSocketFd() { return (clientSocket);}
