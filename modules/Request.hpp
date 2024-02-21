#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

#include "Config.hpp"
#include "Client.hpp"
#include "./enum/Enums.hpp"

class Request {
	private:
		char buf[BUF_SIZE];
		int readenContentLength;
		string leftOverBuffer;
		map<string, string> properties;
		int clientSocketFd;
		
		ERequestStatus status;
	public:
		Request(const int& clientSocketFd);
		~Request();

		void init();
		void parseRequest(Client& client);
		void parseStartLine(istringstream& iss);
		void parseHeader(istringstream& iss);
		void parseBody(istringstream& iss);
		void parseDefaultBody();
		void parseChunkedBody();
		void parseBinaryBody();// cgi

		bool checkCRLF(const istringstream& iss);
		void readRestHttpMessage();
		void handleParsedRequest();
};

#endif