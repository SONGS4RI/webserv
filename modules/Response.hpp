#pragma once
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
#include "Utils.hpp"
#include "ResponseBody.hpp"
#include "./enum/Enums.hpp"

using namespace std;

class Response {
	private:
		EResponseStatus status;
		size_t			nextIdx;
		string			statusLine;
		string			header;
		ResponseBody*	body;
		
		void	setStatusLine(const StatusCode& statusCode);
		void	setHeader();
	public:
		bool	isDone();
		Response(ResponseBody* _body);
		void	writeToSocket(int fd);
		EResponseStatus getStatus() {return (status);};
		~Response();		
};

#endif