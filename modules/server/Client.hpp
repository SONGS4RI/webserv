#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

using namespace std;

class Server;
class Request;

class Client {
	private:
		const Server&	myServer;
		Request *request;
		Response *response;
		int clientSocket;
		int pipeFd;
		int pid;
	public:
		Client(int _clientSocket, const Server& _server);
		~Client();

		void setRequest(Request* _request);
		void setResponse(Response* _response);
		void setPipeFd(const int& _pipeFd);
		void setPid(const int& _pid);

		const Server&	getServer() const;
		Request* getRequest();
		Response* getResponse();
		const int& getSocketFd() const;
		const int& getPipeFd();
		const int& getPid();
};

#endif