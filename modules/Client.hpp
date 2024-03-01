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
		Client();
		Client(int _clientSocket, const Server& _server);
		Client(const Client& obj);
		~Client();
		Client& operator=(const Client& obj);

		Request* getCurReqeust();
		Response* getCurResponse();
		void addResponse(Response response);
		const int& getSocketFd();
};

#endif