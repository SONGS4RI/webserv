#include "RequestBody.hpp"

RequestBody::RequestBody() {}

void RequestBody::init(const string& contentType, const size_t& contentLength, const string& boundary) {
	this->contentType = contentType;
	this->contentLength = contentLength;
	this->chunkedStatus = LENGTH;
	this->body = "";
	this->boundary = boundary;
}

RequestBody::~RequestBody() {}


const string& RequestBody::getContentType() const {
	return contentType;
}

const size_t& RequestBody::getContentLength() const {
	return contentLength;
}

const EChunkedStatus& RequestBody::getChunkedStatus() const {
	return chunkedStatus;
}

const string& RequestBody::getBody() const {
	return body;
}

const string& RequestBody::getBoundary() const {
	return boundary;
}

void RequestBody::setContentLength(const size_t& contentLength) {
	this->contentLength = contentLength;
}

void RequestBody::setChunkedStatus(const EChunkedStatus& chunkedStatus) {
	this->chunkedStatus = chunkedStatus;
}

void RequestBody::addBody(const char* str, const size_t& readCnt) {
	int prevSize = body.size();
	body.resize(prevSize + readCnt);
	for (size_t i = 0; i < readCnt; i++) {
		body[prevSize + i] = *(str + i);
	}
}