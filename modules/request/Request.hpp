#pragma once
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "RequestBody.hpp"
#include "../parseConfig/Config.hpp"
#include "../server/Client.hpp"
#include "../utils/Enums.hpp"
#include "../utils/StatusCode.hpp"

#define TMP_SIZE 1024////config max body size로 교체해야함

class Client;

class Request {
	private:
		char buf[TMP_SIZE];
		size_t readenContentLength;
		string leftOverBuffer;
		RequestBody* body;
		map<string, string> properties;
		const Client* client;
		ERequestStatus status;
		StatusCode statusCode;
		istringstream readbuf;
		bool isUrlIndex;
	public:
		Request();
		Request(const Client* client);
		~Request();

		void parseRequest();
		void parseStartLine();
		void parseHeader();
		void parseBody();
		void parseDefaultBody();
		void parseChunkedBody();

		bool getLineAndCheckCRLF(const char& deli);
		bool checkCRLF();
		void checkHeaderLineBlock(const string& key, istringstream& iss);

		const ERequestStatus& getStatus() const;
		const map<string, string>& getProperties() const;
		RequestBody* getBody() const;
		const StatusCode& getStatusCode() const;
		const bool& getIsUrlIndex() const;
};

#endif