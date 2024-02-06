#include <unistd.h>

#include "Request.hpp"

using namespace std;

void Request::parseRequest(Client& client) {
	int n = read(client.getSocketFd(), buf, sizeof(buf));
	if (n <= 0) {
		// 무언가 처리
	}
	string curParsing = leftOverBuffer + string(buf);
	Request* curRequest = client.getCurReqeust();
	if (curRequest == NULL) {
		curRequest = client.addRequest(new Request());
	}
	while (1/* curParsing 처리 */) {
		switch (status) {
			case START_LINE :
				/* code */
				break;
			case HEADER :
				/* code */
				break;
			case BODY :
				/* code */
				break;
			case PARSE_DONE :
				client.addResponse(new Response());
				client.deleteParsedRequest();
				break;
			default :// ERROR
				// rfc 대로 안된 request 문
				break;
		}
	}
}