#include <unistd.h>
#include <sstream>
#include <iostream>

#include "Request.hpp"

using namespace std;

Request* request = NULL;

Request::Request(const int& clientSocketFd) {
	init();
	this->clientSocketFd = clientSocketFd;
}

void Request::init() {
	memset(buf, 0, BUF_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = "";
	this->properties.clear();
	this->status = ERequestStatus::START_LINE;
}

Request* Request::getRequest(const int& clientSocketFd) {
	if (request == NULL) {
		request = new Request(clientSocketFd);
	}
	return request;
}

bool Request::checkCRLF(const istringstream& iss) {
	int readCnt = iss.gcount();
	bool isValid = (readCnt > 1 && buf[readCnt - 2] == '\r' && buf[readCnt - 1] == '\n');
	if (isValid) {
		return true;
	}
	// 다 읽은 경우도 고려해야 할 수도 있음
	if (!iss.eof()) {// crlf 로 안 끝났는데, 끝까지 읽지 않은 경우
		leftOverBuffer = string(buf, iss.gcount());
	}
	return false;
}

void Request::readRestHttpMessage() {
	int n;
	while (n = read(clientSocketFd, buf, BUF_SIZE) > 0) {

	}
}

void Request::parseStartLine(const istringstream& iss) {
	if (!checkCRLF(iss)) {
		// eof 면??
		return ;
	}
	istringstream startLine(buf, iss.gcount() - 2);
	string method, requestUrl, httpVersion;
	// GET /local/tmp HTTP/1.1 형식인지 체크
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()]) {// 제대로 된 형식 이 아니라면
		throw new exception;
	}
	// 검증 해야함
	properties["method"] = method;
	properties["requestUrl"] = requestUrl;
	properties["httpVersion"] = httpVersion;
	status = ERequestStatus::HEADER;
}

void Request::parseHeader(const istringstream& iss) {
	if (!checkCRLF(iss)) {
		return ;
	}
	if (iss.gcount() == 2) {
		// 메서드별 필수 헤더 값 있는지 체크 없으면 throw
		status = ERequestStatus::BODY;
		return ;
	}
	// 한줄씩 읽은거 properties 에 저장


}

void Request::parseRequest(Client& client) {
	int n = read(client.getSocketFd(), buf, sizeof(buf));
	if (n <= 0) {
		// 무언가 처리
		return ;
	}
	string curParsing(leftOverBuffer), type;
	curParsing.resize(leftOverBuffer.size() + n);

	for (int i = 0; i < n; i++) {
		curParsing[leftOverBuffer.size() + i] = buf[i];
	}
	istringstream iss(curParsing);

	try {
		while (!iss.eof() && status != ERequestStatus::PARSE_DONE) {
			if (status == ERequestStatus::HEADER) {
				iss.getline(buf, BUF_SIZE);
				parseHeader(iss);
			} else if (status == ERequestStatus::BODY) {
				type = properties["type"];
				if (type == "default") {

				} else if (type == "multipart/form-data") {
					
				} else if (type == "binary") {// Transfer-Encoding: chunked

				} else {
					throw new exception;
				}
			} else if (status == ERequestStatus::START_LINE) {
				iss.getline(buf, BUF_SIZE);
				parseStartLine(iss);
			}
		}
		if (status == ERequestStatus::PARSE_DONE) {
			if (readenContentLength != atoi(properties["contentLength"].c_str())) {
				throw new exception;
			}
			client.addResponse(new Response());
		}
	} catch(const std::exception& e) {
		// error response 생성
	}
}