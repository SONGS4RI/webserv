#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "EnumsTest.hpp"
#include "RequestBodyTest.hpp"

using namespace std;

#define BUF_SIZE 1024
#define TMP_SIZE 1024///////////

class RequestTest {
	private:
		char buf[TMP_SIZE];
		size_t readenContentLength;
		string leftOverBuffer;
		RequestBodyTest body;
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

		bool getLineAndCheckCRLF(const char& deli);
		bool checkCRLF();
		void checkHeaderLineBlock(const string& key, istringstream& iss);
		void readRestHttpMessage();
		void handleParsedRequest();
		void toString();
};

#endif