#pragma once

#include <string>
#include <vector>
#include <map>
#include "Config.hpp"
#include "Client.hpp"

enum ERequestStatus {
	START_LINE,
	HEADER,
	BODY,
	PARSE_DONE,
	ERROR
};

class Request {
	private:
		ERequestStatus status;
		string leftOverBuffer;
		unordered_map<string, string> properties;
		char buf[BUF_SIZE];
		int readenContentLength;
	public:
		Request(const string& leftOverBuffer);
		// Request& operator=(const Request& ref);
		~Request();

		void parseRequest(Client& client);
		void parseStartLine(const istringstream& iss);
		void parseHeader();
		void parseDefaultBody();
		void parseChunkedBody();
		void parseBinaryBody();// cgi

		bool checkCRLF(const istringstream& iss);
		void readRestHttpMessage(istringstream& iss);
		void handleParsedRequest();
};