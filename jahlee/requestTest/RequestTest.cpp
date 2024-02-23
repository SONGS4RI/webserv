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
	cout << "readenMessageLength" << ":" << readenMessageLength << "\n";
	cout << "readenContentLength" << ":" << readenContentLength << "\n";
	cout << "[BODY]\n";
	cout << body << "\n";
}

void RequestTest::init() {
	memset(buf, 0, BUF_SIZE);
	this->readenContentLength = 0;
	this->leftOverBuffer = "";
	this->body = "";
	properties.clear();
	this->status = START_LINE;
}

bool RequestTest::checkCRLF() {
	int readCnt = readbuf.gcount();
	readenMessageLength += readCnt;
	cout << string(buf) << "\n";
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
	readbuf.getline(buf, BUF_SIZE * 2);
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
	HTTPInfoTest::isValidStartLine(method, requestUrl, httpVersion);
	properties[METHOD] = method;
	properties[REQUEST_URL] = requestUrl;
	status = HEADER;
}

void RequestTest::checkHeaderLineBlock(const string& key, istringstream& block) {
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

void RequestTest::parseHeader() {
	readbuf.getline(buf, BUF_SIZE * 2);
	if (!checkCRLF()) {
		return ;
	}
	if (readbuf.gcount() == 2) {
		HTTPInfoTest::isValidHeaderField(properties);
		status = BODY;
		return ;
	}
	// 한줄씩 읽은거 properties 에 저장
	istringstream headerField(buf);
	headerField.getline(buf, BUF_SIZE * 2, ':');
	string key(buf), value, rest;
	for (size_t i = 0; i < key.size(); i++) {
		key[i] = tolower(key[i]);
	}
	if (key == HOST || key == CONTENT_LENGTH || key == TRANSFER_ENCODING) {
		checkHeaderLineBlock(key, headerField);
	} else if (key == CONTENT_TYPE) {
		headerField.getline(buf, BUF_SIZE * 2, ';');
		istringstream block(buf);
		checkHeaderLineBlock(key, block);
		headerField.getline(buf, BUF_SIZE * 2);
		if (headerField.gcount()) {
			block.str(buf);
			block.getline(buf, BUF_SIZE * 2, '=');
			key = buf;
			checkHeaderLineBlock(key, block);
		}
	}
}

void RequestTest::parseDefaultBody() {
	size_t contentLength = atoi(properties[CONTENT_LENGTH].c_str());
	size_t targetLength = contentLength - readenContentLength;

	readbuf.read(buf, BUF_SIZE * 2);
	body.resize(body.size() + readbuf.gcount());
	for (int i = 0; i < readbuf.gcount(); i++) {
		body[readenContentLength + i] = buf[i];
	}
	readenContentLength += readbuf.gcount();
	if (readenContentLength == contentLength && readbuf.eof()) {
		status = PARSE_DONE;
	}
	if (targetLength < 0) {
		throw new exception;
	}
}

void RequestTest::parseChunkedBody() {
	while (!readbuf.eof()) {
		// 크기
		readbuf.getline(buf, BUF_SIZE * 2);
		if (!checkCRLF()) {
			throw new exception;
		}
		long contentLength = UtilsTest::hexToDecimal(buf);// int contentLength = hex -> 십진수
		cout << "contentLength:" << contentLength << "\n";
		if (!contentLength) {
			status = PARSE_DONE;
			return ;
		}
		// 바디
		readbuf.getline(buf, BUF_SIZE * 2);
		if (!checkCRLF() || readbuf.gcount() - 2 != contentLength) {
			throw new exception;
		}
		body.resize(body.size() + readbuf.gcount() - 2);
		for (int i = 0; i < readbuf.gcount() - 2; i++) {
			body[readenContentLength + i] = buf[i];
		}
		readenContentLength += readbuf.gcount() - 2;
		// 개행
		readbuf.getline(buf, BUF_SIZE * 2);
		if (!checkCRLF() || readbuf.gcount() != 2) {
			throw new exception;
		}
	}
}

void RequestTest::parseBody() {
	string method = properties[METHOD];
	if (method == GET || method == DELETE) {
		status = PARSE_DONE;
		return ;
	}
	if (properties[CONTENT_LENGTH] != "") {
		parseDefaultBody();
	} else if (properties[TRANSFER_ENCODING] == CHUNKED) {
		cout << "chunked parsing\n";
		parseChunkedBody();
	}
}

void RequestTest::parseRequest(int fd) {
	cout << "parse START\n";
	readenMessageLength = 0;
	int n = read(fd, buf, BUF_SIZE);
	if (n < 0) {
		// 무언가 처리
		return ;
	}
	cout << "leftOverBuffer:" << leftOverBuffer << "\n";
	string curParsing(leftOverBuffer);
	curParsing.resize(curParsing.size() + n);

	for (int i = 0; i < n; i++) {
		curParsing[leftOverBuffer.size() + i] = buf[i];
	}
	readbuf.clear();
	readbuf.str(curParsing);
	try {
		while (!readbuf.eof()) {
			if (status == HEADER) {
				cout << "parseHeader START\n";
				parseHeader();
				cout << "parseHeader DONE\n";
			} else if (status == BODY) {
				cout << "parseBody START\n";
				parseBody();
				cout << "parseBody DONE\n";
			} else if (status == START_LINE) {
				cout << "parseStartLine START\n";
				parseStartLine();
				cout << "parseStartLine DONE\n";
			} else if (status == PARSE_DONE) {
				cout << "PARSE_DONE START\n";
				if (properties[TRANSFER_ENCODING] != CHUNKED && readenContentLength != (size_t)atoi(properties[CONTENT_LENGTH].c_str())) {// contentLength 있는 경우 없는경우 체크
					throw new exception;
				}
				cout << "PARSE_DONE DONE\n";
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