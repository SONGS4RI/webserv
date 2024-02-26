#pragma once
#ifndef REQUESTBODYTEST_HPP
#define REQUESTBODYTEST_HPP

#include <string>

#include "EnumsTest.hpp"

using namespace std;

class RequestBodyTest {
	private:
		string contentType;
		size_t contentLength;
		EChunkedStatus chunkedStatus;
		string body;
	public:
		~RequestBodyTest();
		void init(const string& contentType, const size_t& contentLength);

		const string& getContentType() const;
		const size_t& getContentLength() const;
		const EChunkedStatus& getChunkedStatus() const;
		const string& getBody() const;

		void setContentLength(const size_t& contentLength);
		void setChunkedStatus(const EChunkedStatus& chunkedStatus);
		void addBody(const char* str, const size_t& readCnt);
};

#endif