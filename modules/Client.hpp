#pragma once

#include <Request.hpp>
#include <Response.hpp>

#include <queue>

class Client {
	private:
		queue<Request> requests;
		queue<Response> responses;
	public:
		Client(/* args */);
		~Client();

};