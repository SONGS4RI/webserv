#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "EnumsTest.hpp"

using namespace std;

#define BUF_SIZE 10

class RequestTest {
	private:
		char buf[1024];
		size_t readenMessageLength;
		size_t readenContentLength;
		string leftOverBuffer;
		string body;
		map<string, string> properties;
		int clientSocketFd;
		ERequestStatus status;
		istringstream readbuf;
	public:
		RequestTest(const int& clientSocketFd);
		~RequestTest();

		void init();
		ERequestStatus getStatus();
		void parseRequest(int fd);
		void parseStartLine();
		void parseHeader();
		void parseBody();
		void parseDefaultBody();
		void parseChunkedBody();

		bool checkCRLF();
		void checkHeaderLineBlock(const string& key, istringstream& iss);
		void readRestHttpMessage();
		void handleParsedRequest();
		void toString();
};

#endif