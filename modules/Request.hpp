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
		size_t readenMessageLength;
		size_t readenContentLength;
		string leftOverBuffer;
		string body;
		map<string, string> properties;
		int clientSocketFd;
		ERequestStatus status;
		istringstream readbuf;
	public:
		Request(const int& clientSocketFd);
		~Request();

		void init();
		void parseRequest(Client& client);
		void parseStartLine();
		void parseHeader();
		void parseBody();
		void parseDefaultBody();
		void parseChunkedBody();
		void parseMPFDBody();// multipart/form-data

		bool checkCRLF();
		void checkHeaderLineBlock(const string& key, istringstream& iss);
		void readRestHttpMessage();
		void handleParsedRequest();
};

#endif