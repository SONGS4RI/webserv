#pragma once
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
#include "Utils.hpp"
#include "./enum/Enums.hpp"

using namespace std;

class Response {
	private:
		//EResponseStatus status;
		string	statusLine;
		string	header;
		string	body;
		string	contentType;
		size_t	contentLength;
		void	setStatusLine(int& statusCode);
		void	setHeader();
	public:
		Response(int statusCode, const string& _contentsType, const string& _body);
		void	writeToSocket(int fd);
		void	printAllInfo();
		~Response();		
};

#endif