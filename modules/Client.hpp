#pragma once

#include <queue>

#include "Request.hpp"
#include "Response.hpp"

class Client {
	private:
		queue<Request> requests;
		queue<Response> responses;
	public:
		Client(/* args */);
		~Client();

};