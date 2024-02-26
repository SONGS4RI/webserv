#pragma once
#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <string>

#include "./enum/Enums.hpp"

using namespace std;

class RequestBody {
	private:
		string contentType;
		size_t contentLength;
		EChunkedStatus chunkedStatus;
		string body;
	public:
		~RequestBody();
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