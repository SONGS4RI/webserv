#pragma once

#include <queue>

#include "Request.hpp"
#include "Response.hpp"

class Client {
	private:
		Request request;
		deque<Response> responses;
		int socket;
	public:
		Client(/* args */);
		~Client();

		Request* getCurReqeust();
		Response* getCurResponse();
		Request* addRequest(Request* request);
		void addResponse(Response* response);
		const int& getSocketFd();
		void deleteParsedRequest();// 동적할당 해제
};