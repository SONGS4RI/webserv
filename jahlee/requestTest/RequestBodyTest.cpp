#include "RequestBodyTest.hpp"

void RequestBodyTest::init(const string& contentType, const size_t& contentLength) {
	this->contentType = contentType;
	this->contentLength = contentLength;
	this->chunkedStatus = LENGTH;
	this->body = "";
}

RequestBodyTest::~RequestBodyTest() {}


const string& RequestBodyTest::getContentType() const {
	return contentType;
}

const size_t& RequestBodyTest::getContentLength() const {
	return contentLength;
}

const EChunkedStatus& RequestBodyTest::getChunkedStatus() const {
	return chunkedStatus;
}

const string& RequestBodyTest::getBody() const {
	return body;
}

void RequestBodyTest::setContentLength(const size_t& contentLength) {
	this->contentLength = contentLength;
}

void RequestBodyTest::setChunkedStatus(const EChunkedStatus& chunkedStatus) {
	this->chunkedStatus = chunkedStatus;
}

void RequestBodyTest::addBody(const char* str, const size_t& readCnt) {
	int prevSize = body.size();
	body.resize(prevSize + readCnt);
	for (size_t i = 0; i < readCnt; i++) {
		body[prevSize + i] = *(str + i);
	}
}