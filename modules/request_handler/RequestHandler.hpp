#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <map>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "../utils/HTTPInfo.hpp"
#include "../request/Request.hpp"
#include "../request/RequestBody.hpp"
#include "../response/ResponseBody.hpp"
#include "../server/SocketManager.hpp"
#include "../server/Client.hpp"
using namespace std;

class RequestHandler {
	private:
		string method;
		string requestUrl;
		RequestBody* requestBody;
		ResponseBody* responseBody;
		Client* client;
		const Config*	config;
		bool isUrlDir;
		bool isUrlIndex;
		char* buf;// 동적인 것으로 변경
		size_t bodyMaxSize;
		void checkResource();
		void handleGet();
		void handleDelete();
		void handlePost();
		void handleCgiExecve();
		void handleCgiRead();
		void handleError(const StatusCode& statusCode);
		void dirListing(const string& source, string& url);
	public:
		RequestHandler(const Request* request, Client* client);
		~RequestHandler();

		ResponseBody* handleRequest();
};

#endif