#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const Request& request) {
	this->method = request.getProperties().find(METHOD)->second;
	this->requestUrl = request.getProperties().find(REQUEST_URL)->second;
	this->body = request.getBody();
}

Response& RequestHandler::handleRequest() {
	Response response;
	try {
		checkResource();
		if (method == GET) {
			handleGet();
		} else if (method == DELETE) {
			handleDelete();
		} else if (body.getContentType() == MULTIPART_FORM_DATA){// POST
			// MULTIPART_FORM_DATA CGI
		} else if (body.getContentType() == APPLICATION_OCTET_STREAM || body.getContentType() == IMAGE_PNG) {
			// handle binary CGI
		} else {// POST 인데 일반 자원
			handlePost();
		}
	} catch(const exception* e) {
		// Handle error
		// return ErrorResponse
	}
	return response;
}

void RequestHandler::handleGet() {
	int fd = open(requestUrl.c_str(), O_RDONLY);
	int n = read(fd, buf, sizeof(buf));
	body.addBody(buf, n);
}

void RequestHandler::checkResource() const {
	struct stat buffer;

	if ((stat(requestUrl.c_str(), &buffer) != 0) ||
		((method == GET || method == DELETE) && S_ISDIR(buffer.st_mode)) ||
		(method == POST && !S_ISDIR(buffer.st_mode))) {
		throw new exception;
	}
}