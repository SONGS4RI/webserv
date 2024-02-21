#include <unistd.h>
#include <sstream>
#include <iostream>

#include "Request.hpp"
#include "HTTPInfo.hpp"

using namespace std;

Request::Request(const int& clientSocketFd) {
	init();
	this->clientSocketFd = clientSocketFd;
}

void Request::init() {
	memset(buf, 0, BUF_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = string(BUF_SIZE, '\0');
	properties.clear();
	this->status = ERequestStatus::START_LINE;
}

bool Request::checkCRLF(const istringstream& iss) {
	int readCnt = iss.gcount();
	bool isValid = (readCnt > 1 && buf[readCnt - 2] == '\r' && buf[readCnt - 1] == '\0');// \n 은 readline 으로 걸러짐
	if (isValid) {
		buf[readCnt - 2] = '\0';
		return true;
	}
	if (iss.eof()) {// crlf 로 안 끝났는데, 끝까지 읽은 경우
		leftOverBuffer = string(buf, iss.gcount());
	} else {
		throw new exception;// exception 도 할당 해제?
	}
	return false;
}

void Request::checkHeaderLineBlock(const string& key, istringstream& block) {
	// 이미 들어온 키인 경우
	if (properties[key].size()) {
		throw new exception;
	}
	string value, rest;
	block >> value >> rest;
	if (value == "" || rest != "") {
		throw new exception;
	}
	properties[key] = value;
}

void Request::readRestHttpMessage() {
	int n;
	while (n = read(clientSocketFd, buf, BUF_SIZE) > 0) {

	}
}

void Request::parseStartLine(istringstream& iss) {
	iss.getline(buf, BUF_SIZE);
	if (!checkCRLF(iss)) {
		// eof 면??
		return ;
	}
	istringstream startLine(buf);
	string method, requestUrl, httpVersion;
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()] != ' ') {// 제대로 된 형식 이 아니라면
		throw new exception;
	}
	HTTPInfo::isValidStartLine(method, requestUrl, httpVersion);
	properties["method"] = method;
	properties["requestUrl"] = requestUrl;
	status = ERequestStatus::HEADER;
}

void Request::parseHeader(istringstream& iss) {
	iss.getline(buf, BUF_SIZE);
	if (!checkCRLF(iss)) {
		return ;
	}
	if (iss.gcount() == 2) {
		HTTPInfo::isValidHeaderField(properties);
		status = ERequestStatus::BODY;
		return ;
	}
	// 한줄씩 읽은거 properties 에 저장
	istringstream headerField(buf);
	headerField.getline(buf, sizeof(buf), ':');
	string key(buf), value, rest;
	for (int i = 0; i < key.size(); i++) {
		key[i] = tolower(key[i]);
	}
	if (key == "host" || key == "content-length" || key == "transfer-encoding") {
		checkHeaderLineBlock(key, headerField);
	} else if (key == "content-type") {
		headerField.getline(buf, sizeof(buf), ';');
		istringstream block(buf);
		checkHeaderLineBlock(key, block);
		headerField.getline(buf, sizeof(buf));
		if (headerField.gcount()) {
			block.str(buf);
			block.getline(buf, sizeof(buf), '=');
			key = buf;
			checkHeaderLineBlock(key, block);
		}
	}
}

void Request::parseBody(istringstream& iss) {
	string method = properties["method"];
	if (method == "GET" || method == "DELETE") {
		status = ERequestStatus::PARSE_DONE;
		return ;
	}
	// content type 별로 읽어야함
	string contentType = properties["content-type"];
	if (contentType == "text/plain" || contentType == "text/html") {

	} else if (contentType == "multipart/form-data") {
		// text/plain: f
		// 
	} else if (contentType == "application/octet-stream" && properties["transfer-encoding"] == "chunked") {

	} else {
		throw new exception;
	}
}

void Request::parseRequest(Client& client) {
	int n = read(client.getSocketFd(), buf, sizeof(buf));
	if (n <= 0) {
		// 무언가 처리
		return ;
	}
	string curParsing(leftOverBuffer);
	curParsing.resize(leftOverBuffer.size() + n);

	for (int i = 0; i < n; i++) {
		curParsing[leftOverBuffer.size() + i] = buf[i];
	}
	istringstream iss(curParsing);

	try {
		while (!iss.eof() && status != ERequestStatus::PARSE_DONE) {
			if (status == ERequestStatus::HEADER) {
				parseHeader(iss);
			} else if (status == ERequestStatus::BODY) {
				parseBody(iss);
			} else if (status == ERequestStatus::START_LINE) {
				parseStartLine(iss);
			} else if (status == ERequestStatus::PARSE_DONE) {
				if (readenContentLength != atoi(properties["content-length"].c_str())) {// contentLength 있는 경우 없는경우 체크
					throw new exception;
				}
				client.addResponse(Response());
			}
		}
	} catch(const std::exception& e) {
		// error response 생성
	}
}