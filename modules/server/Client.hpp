#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
//생성자에서 fcntl 실패했을 경우 처리해야할듯.

using namespace std;

class Server;
class Request;

class Client {
	private:
		const Server&	myServer;
		Request *request;
		Response *response;
		int clientSocket;
	public:
		Client(int _clientSocket, const Server& _server);
		~Client();

		void setRequest(Request* _request);
		void setResponse(Response* _response);

		const Server&	getServer() const;
		Request* getReqeust();
		Response* getResponse();
		const int& getSocketFd();
};

#endif