#include <unistd.h>
#include <sstream>
#include <iostream>

#include "Request.hpp"
#include "../utils/HTTPInfo.hpp"
#include "../utils/Utils.hpp"
#include "../utils/StatusCode.hpp"

using namespace std;

Request::Request(const int& clientSocketFd) {
	init();
	this->clientSocketFd = clientSocketFd;
}

Request::~Request() {
	delete body;
}

void Request::init() {
	memset(buf, 0, TMP_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = "";
	this->body = new RequestBody();
	properties.clear();
	this->status = START_LINE;
	this->statusCode = StatusCode();
}

bool Request::getLineAndCheckCRLF(const char& deli) {//////////////////////////////////
	readbuf.getline(buf, TMP_SIZE, deli);
	return checkCRLF();
}

bool Request::checkCRLF() {
	int readCnt = readbuf.gcount();
	bool isValid = (readCnt > 1 && buf[readCnt - 2] == '\r' && buf[readCnt - 1] == '\0');// \n 은 readline 으로 걸러짐
	if (isValid) {
		buf[readCnt - 2] = '\0';
		return true;
	}
	if (readbuf.eof()) {// crlf 로 안 끝났는데, 끝까지 읽은 경우
		leftOverBuffer = string(buf, readbuf.gcount());
	} else {
		throw StatusCode(500, INTERVER_SERVER_ERROR);
	}
	return false;
}

void Request::readRestHttpMessage() {
	// int n;
	// while (n = read(clientSocketFd, buf, BUF_SIZE) > 0) {

	// }
}

void Request::parseStartLine() {
	if (!getLineAndCheckCRLF('\n')) {
		return ;
	}
	istringstream startLine(buf);
	string method, requestUrl, httpVersion;
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()] != ' ') {// 제대로 된 형식 이 아니라면
		throw StatusCode(400, BAD_REQUEST);
	}
	HTTPInfo::isValidStartLine(method, requestUrl, httpVersion);
	properties[METHOD] = method;
	properties[REQUEST_URL] = requestUrl;
	status = HEADER;
	cout << "StartLine DONE\n";
}

void Request::checkHeaderLineBlock(const string& tmpKey, istringstream& block) {
	istringstream keyblock(tmpKey);
	string key, value, rest;
	keyblock >> key >> rest;
	// 이미 들어온 키이거나 잘못된 형식
	if (properties[key].size() || rest.size()) {
		throw StatusCode(400, BAD_REQUEST);
	}
	block >> value >> rest;
	if (value == "" || rest.size()) {
		throw StatusCode(400, BAD_REQUEST);
	}
	block.clear();
	block.str(key);
	properties[key] = value;
}

void Request::parseHeader() {
	if (!getLineAndCheckCRLF('\n')) {
		return ;
	}
	if (readbuf.gcount() == 2) {
		HTTPInfo::isValidHeaderField(properties);
		status = BODY;
		size_t contentLength = properties[CONTENT_LENGTH] != "" ? atoi(properties[CONTENT_LENGTH].c_str()) : 0;
		body->init(properties[CONTENT_TYPE], contentLength);
		cout << "Header DONE\n";
		return ;
	}
	// 한줄씩 읽은거 properties 에 저장
	istringstream headerField(buf);
	headerField.getline(buf, TMP_SIZE, ':');
	string key(buf);
	for (size_t i = 0; i < key.size(); i++) {
		key[i] = tolower(key[i]);
	}
	if (key == HOST || key == CONTENT_LENGTH || key == TRANSFER_ENCODING) {
		checkHeaderLineBlock(key, headerField);
	} else if (key == CONTENT_TYPE) {
		headerField.getline(buf, TMP_SIZE, ';');
		istringstream block(buf);
		checkHeaderLineBlock(key, block);
		headerField.getline(buf, TMP_SIZE);
		if (headerField.gcount()) {
			block.clear();
			block.str(buf);
			block.getline(buf, TMP_SIZE, '=');
			key = buf;
			checkHeaderLineBlock(key, block);
		}
	}
}

void Request::parseDefaultBody() {
	size_t contentLength = body->getContentLength();
	size_t targetLength = contentLength - readenContentLength;
	cout << "contentLength:" << contentLength << "\n";
	cout << "targetLength:" << targetLength << "\n";
	readbuf.read(buf, TMP_SIZE);
	body->addBody(buf, readbuf.gcount());
	readenContentLength += readbuf.gcount();
	cout << "readenContentLength:" << readenContentLength << "\n";
	if (readenContentLength == contentLength && readbuf.eof()) {
		status = PARSE_DONE;
		cout << "Body DONE\n";
	}
	if (targetLength < 0) {
		throw StatusCode(400, BAD_REQUEST);
	}
}

void Request::parseChunkedBody() {
	while (!readbuf.eof()) {
		if (body->getChunkedStatus() == LENGTH) {
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			body->setChunkedStatus(DATA);
			body->setContentLength(Utils::hexToDecimal(buf));// int contentLength = hex -> 십진수
			if (!body->getContentLength()) {
				status = PARSE_DONE;
				cout << "Body DONE\n";
				return ;
			}
		} else if (body->getChunkedStatus() == DATA) {
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() - 2 != (long)body->getContentLength()) {
				throw StatusCode(400, BAD_REQUEST);
			}
			body->setChunkedStatus(CRLF);
			body->addBody(buf, readbuf.gcount() - 2);
			readenContentLength += readbuf.gcount() - 2;
		} else {// CRLF
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() != 2) {
				throw StatusCode(400, BAD_REQUEST);
			}
			body->setChunkedStatus(LENGTH);
		}
	}
}

void Request::parseBody() {
	string method = properties[METHOD];
	if (method == GET || method == DELETE) {
		status = PARSE_DONE;
		cout << "Body DONE\n";
		return ;
	}
	if (properties[CONTENT_LENGTH] != "") {
		parseDefaultBody();
	} else if (properties[TRANSFER_ENCODING] == CHUNKED) {
		parseChunkedBody();
	}
}

void Request::parseRequest(Client& client) {
	int n = read(client.getSocketFd(), buf, BUF_SIZE);
	if (n < 0) {
		// 무언가 처리
		return ;
	}
	string curParsing(leftOverBuffer);
	curParsing.resize(curParsing.size() + n);

	for (int i = 0; i < n; i++) {
		curParsing[leftOverBuffer.size() + i] = buf[i];
	}
	leftOverBuffer.clear();
	readbuf.clear();
	readbuf.str(curParsing);
	try {
		while (!readbuf.eof()) {
			if (status == HEADER) {
				parseHeader();
			} else if (status == BODY) {
				parseBody();
			} else if (status == START_LINE) {
				parseStartLine();
			} else if (status == PARSE_DONE) {
				if (properties[TRANSFER_ENCODING] != CHUNKED && readenContentLength != body->getContentLength()) {// contentLength 있는 경우 없는경우 체크
					throw StatusCode(400, BAD_REQUEST);
				}
				// Response 만들기전 요청 처리
				break ;
			}
		}
	} catch(const StatusCode& sc) {
		statusCode = sc;
	}
}

const ERequestStatus& Request::getStatus() const { return (status);}

const map<string, string>& Request::getProperties() const { return (properties);}

RequestBody* Request::getBody() const { return (body);}

const StatusCode& Request::getStatusCode() const { return (statusCode);}