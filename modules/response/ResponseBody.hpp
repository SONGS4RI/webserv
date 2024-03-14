#pragma once
#ifndef RESPONSEBODY_HPP
#define RESPONSEBODY_HPP

#include <string>

#include "../utils/Enums.hpp"
#include "../utils/StatusCode.hpp"

using namespace std;

class ResponseBody {
	private:
		StatusCode statusCode;
		string contentType;
		size_t contentLength;
		string location;
		string body;
		ResponseBody();
	public:
		ResponseBody(const StatusCode& statusCode);
		~ResponseBody();
		ResponseBody(const ResponseBody& obj);
		const ResponseBody& operator=(const ResponseBody& obj);

		const StatusCode& getStatusCode() const;
		const string& getContentType() const;
		const size_t& getContentLength() const;
		const string& getBody() const;
		const string& getLocation() const;

        void setStatusCode(const StatusCode& statusCode);
        void setContentType(const string& contentType);
        void setContentLength(const size_t& contentLength);
        void setBody(const char* str, const size_t& readCnt);
		void setLocation(const string& _location);
};

# endif