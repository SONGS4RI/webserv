#pragma once
#ifndef RESPONSEBODY_HPP
#define RESPONSEBODY_HPP

#include <string>

#include "./enum/Enums.hpp"

using namespace std;

class ResponseBody {
	private:
		size_t statusCode;
		string contentType;
		size_t contentLength;
		string body;
	public:
		//ResponseBody();
		ResponseBody(size_t code, string _type, string _body);
		//~ResponseBody();
		//ResponseBody(const ResponseBody& obj);
		// const ResponseBody& operator=(const ResponseBody& obj);
		// void init(const string& contentType, const size_t& contentLength);

		const size_t& getStatusCode() const;
		const string& getContentType() const;
		const size_t& getContentLength() const;
		const string& getBody() const;

		// void setContentLength(const size_t& contentLength);
		// void setChunkedStatus(const EChunkedStatus& chunkedStatus);
		// void addBody(const char* str, const size_t& readCnt);
};

#endif