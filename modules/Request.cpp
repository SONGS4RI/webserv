#include <unistd.h>
#include <sstream>

#include "Request.hpp"

using namespace std;

Request::Request(const string& leftOverBuffer) {
	this->status = START_LINE;
	this->leftOverBuffer = leftOverBuffer;
	this->readenContentLength = 0;
}

bool Request::checkCRLF(const istringstream& iss) {
	int readCnt = iss.gcount();
	bool isValid = (readCnt > 1 && buf[readCnt - 2] == '\r' && buf[readCnt - 1] == '\n');
	if (isValid) {
		return true;
	}
	if (!iss.eof()) {// crlf 로 안 끝났는데, 끝까지 읽지도 않은 경우
		status = ERROR;
	} else {// 다읽었는데 아직 http 메세지를 다읽지 않았다면
		leftOverBuffer = string(buf, iss.gcount());
	}
	return false;
}

void Request::readRestHttpMessage(istringstream& iss) {
	iss.getline(buf, BUF_SIZE);
}

void Request::parseStartLine(const istringstream& iss) {
	if (!checkCRLF(iss)) {
		return ;
	}
	istringstream startLine(buf, iss.gcount() - 2);
	string method, requestUrl, httpVersion;
	// GET /local/tmp HTTP/1.1 형식인지 체크
	startLine >> method >> requestUrl >> httpVersion;
	if (!startLine.eof() || startLine.fail() || buf[method.size()] != ' ' ||
		buf[method.size() + 1 + requestUrl.size()]) {// 제대로 된 형식 이 아니라면

	}
	properties["method"] = method;
	properties["requestUrl"] = requestUrl;
	properties["httpVersion"] = httpVersion;
	status = HEADER;
}

void Request::parseHeader() {
	if (buf[0] == '\r') {// \r \n 둘 다 체크해서 넣어 줘야함, 수정 필요
		status = BODY;
		return ;
	}
	// 한줄씩 읽은거 properties 에 저장
}

void Request::parseRequest(Client& client) {
	int n = read(client.getSocketFd(), buf, sizeof(buf));
	if (n <= 0) {
		// 무언가 처리
	}
	string curParsing(leftOverBuffer), type;
	curParsing.resize(leftOverBuffer.size() + n);

	for (int i = 0; i < n; i++) {
		curParsing[leftOverBuffer.size() + i] = buf[i];
	}
	istringstream iss(curParsing);
	while (!iss.eof()) {
		switch (status) {
			case START_LINE :
				iss.getline(buf, BUF_SIZE);
				parseStartLine(iss);
				break;
			case HEADER :
				iss.getline(buf, BUF_SIZE);
				parseHeader();
				break;
			case BODY :
				iss.getline(buf, BUF_SIZE);
				readenContentLength += iss.gcount();
				type = properties["type"];
				if (type == "default") {

				} else if (type == "chunked") {
					
				} else if (type == "binary") {

				} else {
					status = ERROR;
				}
				break;
			case PARSE_DONE :
				if (readenContentLength != stoi(properties["contentLength"])) {
					status = ERROR;
					break;
				}
				client.addResponse(new Response());
				// 만약 아직 파싱안한거 들고있으면 전달해야함
				client.deleteParsedRequest();
				break;
			default :// ERROR
				// rfc 대로 안된 request 문
				// 에러인 경우 처리 남은 소켓의 메세지 처리, 및 에러 페이지 처리등
				break;
		}
	}
}