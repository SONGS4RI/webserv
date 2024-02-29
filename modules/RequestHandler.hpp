#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <map>
#include <sys/stat.h>

#include "HTTPInfo.hpp"
#include "Request.hpp"
#include "RequestBody.hpp"
#include "ResponseBody.hpp"
using namespace std;

class RequestHandler {
	private:
		string method;
		string requestUrl;
		RequestBody* requestbody;
		ResponseBody* responseBody;
		char buf[1024];// 동적인 것으로 변경
		void checkResource() const;
		void handleGet();
		void handleDelete();
		void handlePost();
		void handleError(const StatusCode& statusCode);
	public:
		RequestHandler(const Request* request);
		~RequestHandler();

		ResponseBody* handleRequest();
};

#endif