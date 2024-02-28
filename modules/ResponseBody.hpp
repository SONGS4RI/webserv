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
		ResponseBody();
		~ResponseBody();
		ResponseBody(const ResponseBody& obj);
		const ResponseBody& operator=(const ResponseBody& obj);

		const size_t& getStatusCode() const;
		const string& getContentType() const;
		const size_t& getContentLength() const;
		const string& getBody() const;

        void setStatusCode(const size_t& statusCode);
        void setContentType(const string& contentType);
        void setContentLength(const size_t& contentLength);
        void setBody(const char* str, const size_t& readCnt);
};

# endif

/*

메서드 관련 405

GET 200 -> body
    30X -> 404 html 리다이렉션
POST 201 -> body X
    400 -> 잘못된 url
DELETE 204 -> body X
    400 -> 잘못된 url

*/