#include <unistd.h>
#include <sstream>
#include <iostream>

#include "Request.hpp"
#include "../utils/HTTPInfo.hpp"
#include "../utils/Utils.hpp"
#include "../utils/StatusCode.hpp"
#include "../server/SocketManager.hpp"
#include "../parseConfig/Config.hpp"

using namespace std;

Request::Request(const Client* client) {
	memset(buf, 0, TMP_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = "";
	this->body = new RequestBody();
	properties.clear();
	this->status = START_LINE;
	this->statusCode = StatusCode();
	this->readbuf.clear();
	this->client = client;
	this->isUrlIndex = false;
}

Request::~Request() {
	if (body != NULL) {
		delete body;
		body = NULL;
	}
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

void Request::parseStartLine() {
	if (!getLineAndCheckCRLF('\n')) {
		return ;
	}
	istringstream startLine(buf);
	string method, requestUrl, httpVersion;
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()] != ' ') {
		throw StatusCode(400, "잘못된 형식");
	}
	const Config& serverConfig = client->getServer().getServerConfig();
	string index = requestUrl == "/" ? serverConfig.getIndex() : serverConfig.getIndex(requestUrl);// 루트 페이지
	if (index != "") {
		isUrlIndex = true;
	}
	HTTPInfo::isValidStartLine(method, requestUrl, httpVersion, &client->getServer().getServerConfig());
	requestUrl = serverConfig.getRoot() + (isUrlIndex ? index : requestUrl);
	properties[METHOD] = method;
	properties[REQUEST_URL] = requestUrl;
	status = HEADER;
	Utils::log("StartLine Parse DONE", GREEN);
}

void Request::checkHeaderLineBlock(const string& tmpKey, istringstream& block) {
	istringstream keyblock(tmpKey);
	string key, value, rest;
	keyblock >> key >> rest;
	// 이미 들어온 키이거나 잘못된 형식
	if (properties[key].size() || rest.size()) {
		throw StatusCode(400, "이미 들어온 키이거나 잘못된 형식");
	}
	block >> value >> rest;
	if (value == "" || rest.size()) {
		throw StatusCode(400, "잘못된 형식");
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
		Utils::log("Header Parse DONE", GREEN);
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
	readbuf.read(buf, TMP_SIZE);
	body->addBody(buf, readbuf.gcount());
	readenContentLength += readbuf.gcount();
	if (readenContentLength == contentLength && readbuf.eof()) {
		status = PARSE_DONE;
		Utils::log("Body Parse DONE", GREEN);
	}
	if (targetLength < 0) {
		throw StatusCode(400, "content-length 불일치");
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
				Utils::log("Body Parse DONE", GREEN);
				return ;
			}
		} else if (body->getChunkedStatus() == DATA) {
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() - 2 != (long)body->getContentLength()) {
				throw StatusCode(400, "content-lenght 불일치");
			}
			body->setChunkedStatus(CRLF);
			body->addBody(buf, readbuf.gcount() - 2);
			readenContentLength += readbuf.gcount() - 2;
		} else {// CRLF
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() != 2) {
				throw StatusCode(400, "마지막  crlf");
			}
			body->setChunkedStatus(LENGTH);
		}
	}
}

void Request::parseBody() {
	string method = properties[METHOD];
	if (method == GET || method == DELETE) {
		status = PARSE_DONE;
		Utils::log("Body Parse DONE", GREEN);
		return ;
	}
	if (properties[CONTENT_LENGTH] != "") {
		parseDefaultBody();
	} else if (properties[TRANSFER_ENCODING] == CHUNKED) {
		parseChunkedBody();
	}
}

void Request::parseRequest() {
	int n = read(client->getSocketFd(), buf, BUF_SIZE);
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
					throw StatusCode(400, "content-length 불일치");
				}
				if (readenContentLength > client->getServer().getServerConfig().getClientMaxBodySize()) {
					throw StatusCode(400, "Max Body Size 초과");
				}
				break ;
			}
		}
	} catch(const StatusCode& sc) {
		Utils::log(sc.getMessage(), RED);
		statusCode = sc;
	}
}

const ERequestStatus& Request::getStatus() const { return (status);}

const map<string, string>& Request::getProperties() const { return (properties);}

RequestBody* Request::getBody() const { return (body);}

const StatusCode& Request::getStatusCode() const { return (statusCode);}

const bool& Request::getIsUrlIndex() const { return (isUrlIndex);};