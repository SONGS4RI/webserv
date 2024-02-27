#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <queue>

#include "Request.hpp"
#include "Response.hpp"

class Client {
	private:
		Request request;
		deque<Response> responses;
		int socket;
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