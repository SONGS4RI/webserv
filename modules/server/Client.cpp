#include "Client.hpp"

Client::Client(int _clientSocket, const Server& _server) :
myServer(_server), request(NULL), response(NULL), clientSocket(_clientSocket), pipeFd(-1), pid(-1) {}

Client::~Client() {
	if (request != NULL) {
		delete request;
		request = NULL;
	}
	if (response != NULL) {
		delete response;
		response = NULL;
	}
	close(clientSocket);
}

void Client::setRequest(Request* _request) { request = _request;}

void Client::setResponse(Response* _response) { response = _response;}

void Client::setPipeFd(const int& _pipeFd) { pipeFd = _pipeFd;}

void Client::setPid(const int& _pid) { pid = _pid;}

const Server&	Client::getServer() const { return(myServer);}

Request* Client::getRequest() {
	if (request == NULL) {
		request = new Request(this);
	}
	return (request);
}

Response* Client::getResponse() { return (response);}

const int& Client::getSocketFd() const { return (clientSocket);}

const int& Client::getPipeFd() { return (pipeFd);}

const int& Client::getPid() { return (pid);}
