#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <queue>

#include "Request.hpp"
#include "Response.hpp"
//생성자에서 fcntl 실패했을 경우 
class Client {
	private:
		Request *request;
		Response *response;
		int clientSocket;
	public:
		Client();
		Client(const Client& obj);
		~Client();
		Client& operator=(const Client& obj);

		Request* getCurReqeust();
		Response* getCurResponse();
		void addResponse(Response response);
		const int& getSocketFd();
};

#endif