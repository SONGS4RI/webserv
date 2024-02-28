#include "ResponseBody.hpp"

ResponseBody::ResponseBody() {}

ResponseBody::~ResponseBody() {}

const size_t& ResponseBody::getStatusCode() const {
	return statusCode;
}
const string& ResponseBody::getContentType() const {
	return contentType;
}

const size_t& ResponseBody::getContentLength() const {
	return contentLength;
}

const string& ResponseBody::getBody() const {
	return body;
}

void ResponseBody::setStatusCode(const size_t& statusCode) {
	this->statusCode = statusCode;
}

void ResponseBody::setContentType(const string& contentType) {
	this->contentType = contentType;
}

void ResponseBody::setContentLength(const size_t& contentLength) {
	this->contentLength = contentLength;
}

void ResponseBody::setBody(const char* str, const size_t& readCnt) {
	this->body.resize(readCnt);
	for (size_t i = 0; i < readCnt; i++) {
		this->body[i] = str[i];
	}
}