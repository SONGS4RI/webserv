#include <unistd.h>
#include <sstream>
#include <iostream>

#include "RequestTest.hpp"
#include "HTTPInfoTest.hpp"
#include "UtilsTest.hpp"

using namespace std;

RequestTest::RequestTest(const int& clientSocketFd) {
	init();
	this->clientSocketFd = clientSocketFd;
}

RequestTest::~RequestTest() {
}

void RequestTest::toString() {

	map<string, string>::iterator iter = properties.begin();
	cout << "[key]     :   [value]\n";
	for (; iter != properties.end(); iter++) {
		cout << iter->first << ":" << iter->second << "\n";
	}
	cout << "readenContentLength" << ":" << readenContentLength << "\n";
	cout << "[BODY]\n";
	cout << body.getBody() << "\n";
}

void RequestTest::init() {
	memset(buf, 0, TMP_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = "";
	this->body = RequestBodyTest();
	properties.clear();
	this->status = START_LINE;
}

bool RequestTest::getLineAndCheckCRLF(const char& deli) {//////////////////////////////////
	readbuf.getline(buf, TMP_SIZE, deli);
	return checkCRLF();
}

bool RequestTest::checkCRLF() {
	int readCnt = readbuf.gcount();
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

void RequestTest::readRestHttpMessage() {
	// int n;
	// while (n = read(clientSocketFd, buf, BUF_SIZE) > 0) {

	// }
}

void RequestTest::parseStartLine() {
	if (!getLineAndCheckCRLF('\n')) {
		return ;
	}
	istringstream startLine(buf);
	string method, requestUrl, httpVersion;
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()] != ' ') {// 제대로 된 형식 이 아니라면
		throw new exception;
	}
	HTTPInfoTest::isValidStartLine(method, requestUrl, httpVersion);
	properties[METHOD] = method;
	properties[REQUEST_URL] = requestUrl;
	status = HEADER;
	cout << "StartLine DONE\n";
}

void RequestTest::checkHeaderLineBlock(const string& tmpKey, istringstream& block) {
	istringstream keyblock(tmpKey);
	string key, value, rest;
	keyblock >> key >> rest;
	// 이미 들어온 키이거나 잘못된 형식
	if (properties[key].size() || rest.size()) {
		throw new exception;
	}
	block >> value >> rest;
	if (value == "" || rest.size()) {
		throw new exception;
	}
	block.clear();
	block.str(key);
	properties[key] = value;
}

void RequestTest::parseHeader() {
	if (!getLineAndCheckCRLF('\n')) {
		return ;
	}
	if (readbuf.gcount() == 2) {
		HTTPInfoTest::isValidHeaderField(properties);
		status = BODY;
		size_t contentLength = properties[CONTENT_LENGTH] != "" ? atoi(properties[CONTENT_LENGTH].c_str()) : 0;
		body.init(properties[CONTENT_TYPE], contentLength);
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

void RequestTest::parseDefaultBody() {
	size_t contentLength = body.getContentLength();
	size_t targetLength = contentLength - readenContentLength;
	cout << "contentLength:" << contentLength << "\n";
	cout << "targetLength:" << targetLength << "\n";
	readbuf.read(buf, TMP_SIZE);
	body.addBody(buf, readbuf.gcount());
	readenContentLength += readbuf.gcount();
	cout << "readenContentLength:" << readenContentLength << "\n";
	if (readenContentLength == contentLength && readbuf.eof()) {
		status = PARSE_DONE;
		cout << "Body DONE\n";
	}
	if (targetLength < 0) {
		throw new exception;
	}
}

void RequestTest::parseChunkedBody() {
	while (!readbuf.eof()) {
		if (body.getChunkedStatus() == LENGTH) {
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			body.setChunkedStatus(DATA);
			body.setContentLength(UtilsTest::hexToDecimal(buf));// int contentLength = hex -> 십진수
			if (!body.getContentLength()) {
				status = PARSE_DONE;
				cout << "Body DONE\n";
				return ;
			}
		} else if (body.getChunkedStatus() == DATA) {
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() - 2 != (long)body.getContentLength()) {
				throw new exception;
			}
			body.setChunkedStatus(CRLF);
			body.addBody(buf, readbuf.gcount() - 2);
			readenContentLength += readbuf.gcount() - 2;
		} else {// CRLF
			if (!getLineAndCheckCRLF('\n')) {
				return ;
			}
			if (readbuf.gcount() != 2) {
				throw new exception;
			}
			body.setChunkedStatus(LENGTH);
		}
	}
}

void RequestTest::parseBody() {
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

void RequestTest::parseRequest(int fd) {
	int n = read(fd, buf, BUF_SIZE);
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
				if (properties[TRANSFER_ENCODING] != CHUNKED && readenContentLength != body.getContentLength()) {// contentLength 있는 경우 없는경우 체크
					throw new exception;
				}
				break ;
			}
		}
	} catch(const std::exception* e) {
		throw e;
	}
}

ERequestStatus RequestTest::getStatus() {
	return status;
}