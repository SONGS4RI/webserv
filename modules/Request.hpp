#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "Config.hpp"
#include "Client.hpp"
#include "RequestBody.hpp"
#include "./enum/Enums.hpp"
#include "StatusCode.hpp"

#define TMP_SIZE 1024////config max body size로 교체해야함

class Request {
	private:
		char buf[TMP_SIZE];
		size_t readenContentLength;
		string leftOverBuffer;
		RequestBody* body;
		map<string, string> properties;
		int clientSocketFd;
		ERequestStatus status;
		StatusCode statusCode;
		istringstream readbuf;
	public:
		Request();
		Request(const int& clientSocketFd);
		~Request();

		void init();
		void parseRequest(Client& client);
		void parseStartLine();
		void parseHeader();
		void parseBody();
		void parseDefaultBody();
		void parseChunkedBody();

		bool getLineAndCheckCRLF(const char& deli);
		bool checkCRLF();
		void checkHeaderLineBlock(const string& key, istringstream& iss);
		void readRestHttpMessage();

		const ERequestStatus& getStatus() const;
		const map<string, string>& getProperties() const;
		RequestBody* getBody() const;
		const StatusCode& getStatusCode() const;
};

#endif