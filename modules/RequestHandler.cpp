#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const Request* request) {
	if (request->getStatus() != ERROR) {
		this->method = request->getProperties().find(METHOD)->second;
		this->requestUrl = request->getProperties().find(REQUEST_URL)->second;
		this->requestbody = request->getBody();
	}
	this->responseBody = new ResponseBody(request->getStatusCode());
}

ResponseBody* RequestHandler::handleRequest() {
	try {
		if (responseBody->getStatusCode().getStatusCode() >= 400) {
			throw responseBody->getStatusCode();
		}
		checkResource();
		if (method == GET) {
			handleGet();
		} else if (method == DELETE) {
			handleDelete();
		} else if (requestbody->getContentType() == MULTIPART_FORM_DATA){// POST
			// MULTIPART_FORM_DATA CGI
		} else {// POST
			handlePost();
		}
	} catch(const StatusCode& statusCode) {
		// Handle error
		handleError(statusCode);
	}
	return responseBody;
}

void RequestHandler::handleGet() {
	int fd = open(requestUrl.c_str(), O_RDONLY);
	int n = read(fd, buf, sizeof(buf));
	if (n < 0) {
		handleError(StatusCode(500, INTERVER_SERVER_ERROR));
		return ;
	}
	responseBody->setStatusCode(StatusCode(200, OK));
	responseBody->setContentType(requestbody->getContentType());
	responseBody->setContentLength(n);
	responseBody->setBody(buf, n);
}

void RequestHandler::handleDelete() {
	if (remove(requestUrl.c_str()) != 0) {
		throw StatusCode(405, FORBIDDEN);
	}
	responseBody->setStatusCode(StatusCode(204, NO_CONTENT));
	responseBody->setContentType(requestbody->getContentType());
}

void RequestHandler::handlePost() {
	string extension;
	if (requestbody->getContentType() == APPLICATION_OCTET_STREAM) {
		extension = ".bin";
	} else if (requestbody->getContentType() == IMAGE_PNG) {
		extension = ".png";
	} else if (requestbody->getContentType() == TEXT_HTML) {
		extension = ".html";
	} else if (requestbody->getContentType() == TEXT_PLAIN) {
		extension = ".txt";
	}
	time_t now = time(0);
	srand(static_cast<unsigned int>(now));
	tm* now_tm = localtime(&now);
    int randomValue = rand() % 100000 + 1;
	char time[26];
	sprintf(time, "%04d%02d%02d_%02d%02d%02d",
                 1900 + now_tm->tm_year, now_tm->tm_mon + 1, now_tm->tm_mday,
                 now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	string fileName = string(time) + "_" + to_string(randomValue) + extension;

    ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if (outFile) {
        outFile.write(requestbody->getBody().c_str(), requestbody->getBody().size());
        outFile.close();
    } else {
       throw StatusCode(500, INTERVER_SERVER_ERROR);
    }
}

void RequestHandler::handleError(const StatusCode& statusCode) {
	responseBody->setStatusCode(statusCode);
	responseBody->setContentType(TEXT_HTML);
	string fileName = to_string(statusCode.getStatusCode()) + ".html";
	int fd = open(fileName.c_str(), O_RDONLY);
	int n = read(fd, buf, sizeof(buf));
	responseBody->setContentLength(n);
	responseBody->setBody(buf, n);
}

void RequestHandler::checkResource() const {
	struct stat buffer;
	if (stat(requestUrl.c_str(), &buffer) != 0) {
		throw StatusCode(404, NOT_FOUND);
	}

	if (((method == GET || method == DELETE) && S_ISDIR(buffer.st_mode)) ||
		(method == POST && !S_ISDIR(buffer.st_mode))) {
		throw StatusCode(400, BAD_REQUEST);
	}
}