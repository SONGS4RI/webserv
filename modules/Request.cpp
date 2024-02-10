#include <unistd.h>
#include <sstream>

#include "Request.hpp"

using namespace std;

void Request::parseStartLine() {
	istringstream iss(buf);
	string method, requestUrl, httpVersion;
	iss >> method >> requestUrl >> httpVersion;
	properties["method"] = method;
	properties["requestUrl"] = requestUrl;
	properties["httpVersion"] = httpVersion;
	status = HEADER;
}

void Request::parseHeader() {
	if (buf[0] == '\r') {
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
	string curParsing = leftOverBuffer + string(buf), type;
	istringstream iss(curParsing);
	Request* curRequest = client.getCurReqeust();
	if (curRequest == NULL) {
		curRequest = client.addRequest(new Request());
	}
	int contentLength = 0;
	while (!iss.eof()) {
		switch (status) {
			case START_LINE :
				iss.getline(buf, BUF_SIZE);
				parseStartLine();
				break;
			case HEADER :
				iss.getline(buf, BUF_SIZE);
				parseHeader();
				break;
			case BODY :
				iss.getline(buf, BUF_SIZE);
				contentLength += iss.gcount();
				type = properties["type"];
				if (type == "default") {

				} else if (type == "chunked") {
					
				} else if (type == "binary") {

				} else {
					// error
				}
				break;
			case PARSE_DONE :
				if (contentLength != stoi(properties["contentLength"])) {
					status = ERROR;
					break;
				}
				client.addResponse(new Response());
				// 만약 아직 파싱안한거 들고있으면 전달해야함
				client.deleteParsedRequest();
				break;
			default :// ERROR
				// rfc 대로 안된 request 문
				break;
		}
	}
}