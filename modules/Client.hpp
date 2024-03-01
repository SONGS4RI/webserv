#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
//생성자에서 fcntl 실패했을 경우 처리해야할듯.

class Server;

class Client {
	private:
		const Server&	myServer;
		Request *request;
		Response *response;
		int clientSocket;
	public:
		Client(int _clientSocket, const Server& _server);
		~Client();

		const Server&	getServer() const;
		Request* getReqeust();
		Response* getResponse();
		void addRequest(Request* _request);
		void addResponse(Response* _response);
		const int& getSocketFd();
};

#endif