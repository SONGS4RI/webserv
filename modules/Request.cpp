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
	this->leftOverBuffer = "";
	this->body = "";
	properties.clear();
	this->status = ERequestStatus::START_LINE;
}

bool Request::checkCRLF() {
	int readCnt = readbuf.gcount();
	readenMessageLength += readCnt;
	bool isValid = (readCnt > 1 && buf[readCnt - 2] == '\r' && buf[readCnt - 1] == '\0');// \n 은 readline 으로 걸러짐
	if (isValid) {
		buf[readCnt - 2] = '\0';
		return true;
	}
	if (readbuf.eof()) {// crlf 로 안 끝났는데, 끝까지 읽은 경우
		leftOverBuffer = string(buf, readbuf.gcount());
	} else {
		throw new exception;// exception 도 할당 해제?
	}
	return false;
}

void Request::readRestHttpMessage() {
	int n;
	while (n = read(clientSocketFd, buf, BUF_SIZE) > 0) {

	}
}

void Request::parseStartLine() {
	readbuf.getline(buf, BUF_SIZE);
	if (!checkCRLF()) {
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
	properties[METHOD] = method;
	properties[REQUEST_URL] = requestUrl;
	status = ERequestStatus::HEADER;
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

void Request::parseHeader() {
	readbuf.getline(buf, BUF_SIZE);
	if (!checkCRLF()) {
		return ;
	}
	if (readbuf.gcount() == 2) {
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
	if (key == HOST || key == CONTENT_LENGTH || key == TRANSFER_ENCODING) {
		checkHeaderLineBlock(key, headerField);
	} else if (key == CONTENT_TYPE) {
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

void Request::parseDefaultBody() {
	size_t contentLength = atoi(properties[CONTENT_LENGTH].c_str());
	size_t targetLength = contentLength - readenContentLength;
	size_t allowedLength = targetLength < BUF_SIZE - readenMessageLength ? targetLength : BUF_SIZE - readenMessageLength;

	readbuf.read(buf, allowedLength);
	body.resize(body.size() + allowedLength);
	for (int i = 0; i < allowedLength; i++) {
		body[readenContentLength + i] = buf[i];
	}
	readenContentLength += allowedLength;
	if (readenContentLength == contentLength && readbuf.eof()) {
		status = PARSE_DONE;
	}
	if (targetLength < 0) {
		throw new exception;
	}
}

void Request::parseMPFDBody() {
	
}

void Request::parseBody() {
	string method = properties[METHOD];
	if (method == GET || method == DELETE) {
		status = ERequestStatus::PARSE_DONE;
		return ;
	}
	if (properties[CONTENT_LENGTH] != "") {
		parseDefaultBody();
	} else if (properties[CONTENT_TYPE] == MULTIPART_FORM_DATA) {
		parseMPFDBody();
	} else if (properties[TRANSFER_ENCODING] == CHUNKED) {
		parseChunkedBody();
	}
}

void Request::parseRequest(Client& client) {
	readenMessageLength = 0;
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
	readbuf.str(curParsing);

	try {
		while (!readbuf.eof()) {
			if (status == ERequestStatus::HEADER) {
				parseHeader();
			} else if (status == ERequestStatus::BODY) {
				parseBody();
			} else if (status == ERequestStatus::START_LINE) {
				parseStartLine();
			} else if (status == ERequestStatus::PARSE_DONE) {
				if (readenContentLength != atoi(properties[CONTENT_LENGTH].c_str())) {// contentLength 있는 경우 없는경우 체크
					throw new exception;
				}
				client.addResponse(Response());
				break ;
			}
		}
	} catch(const std::exception& e) {
		// error response 생성
	}
}