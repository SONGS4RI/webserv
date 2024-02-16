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
		static Request* request;
		string leftOverBuffer;
		unordered_map<string, string> properties;
		int clientSocketFd;
		ERequestStatus status;

		Request(const int& clientSocketFd);
	public:
		~Request();

		static Request* getRequest(const int& clientSocketFd);
		void init();
		void parseRequest(Client& client);
		void parseStartLine(const istringstream& iss);
		void parseHeader(const istringstream& iss);
		void parseDefaultBody();
		void parseChunkedBody();
		void parseBinaryBody();// cgi

		bool checkCRLF(const istringstream& iss);
		void readRestHttpMessage();
		void handleParsedRequest();
};

#endif