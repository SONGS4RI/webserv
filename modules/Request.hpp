#pragma once

#include <string>

#include "Config.hpp"
#include "Client.hpp"

enum ERequestStatus {
	START_LINE,
	HEADER,
	BODY,
	PARSE_DONE,
	ERROR
};

enum EType {
	DEFAULT,
	CHUNKED,
	BINARY
};

class Request {
	private:
		ERequestStatus status;
		EType bodyType;
		string leftOverBuffer;
		char buf[BUF_SIZE];
	public:
		Request(/* args */);
		// Request& operator=(const Request& ref);
		~Request();

		void parseRequest(Client& client);
		void handleParsedRequest();
};