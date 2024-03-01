
#include "Response.hpp"
#include "Config.hpp"

Response::Response(ResponseBody* _body) :
 status(RP_STATUS_LINE), nextIdx(0) , statusLine("HTTP/1.1 "), body(_body){
	setStatusLine(_body->getStatusCode());
	if (_body->getStatusCode().getStatusCode() == 200){
		setHeader();
	}
}


void	Response::writeToSocket(int fd) {
	size_t	writeAmount;
	size_t	writtenLength = 0;
	
	while (BUF_SIZE > writtenLength && status != RP_DONE) {
		switch (status) {
			case RP_STATUS_LINE:
				writeAmount = (statusLine.size() - nextIdx < BUF_SIZE - writtenLength ? statusLine.size() - nextIdx : BUF_SIZE - writtenLength);
				write(fd, &statusLine[nextIdx], writeAmount);
				nextIdx += writeAmount;
				writtenLength += writeAmount;
				if (nextIdx == statusLine.size()) {
					status = RP_HEADER;
					nextIdx = 0;
				}
				break;
			case RP_HEADER:
				writeAmount = (header.size() - nextIdx < BUF_SIZE - writtenLength ? header.size() - nextIdx : BUF_SIZE - writtenLength);
				write(fd, &header[nextIdx], writeAmount);
				nextIdx += writeAmount;
				writtenLength += writeAmount;
				if (nextIdx == header.size()) {
					status = RP_BODY;
					nextIdx = 0;
				}
				break;
			case RP_BODY:
				writeAmount = (body->getBody().size() - nextIdx < BUF_SIZE - writtenLength ? body->getBody().size() - nextIdx : BUF_SIZE - writtenLength);
				write(fd, &body->getBody()[nextIdx], writeAmount);
				nextIdx += writeAmount;
				writtenLength += writeAmount;
				if (nextIdx == body->getBody().size()) {
					status = RP_DONE;
					nextIdx = 0;
				}
				break;
			case RP_DONE:
				break ;
			default:
				break;
		}
	}
}

bool	Response::isDone() {
	if (status == RP_DONE) {
		return (true);
	}
	return (false);
}

Response::~Response(){
	delete body;
}
/* 수정 요망 !!! */
//확실히 없는경우 204 201 각종 에러일때, POST, DELETE. 리다이렉션 할때 헤더에 로케이션 들어갈 예정.
//콘텐츠타입, 길이는 GET 200에서만 있음
void	Response::setHeader() {
	header.reserve(200);
	header += "Content-Type: ";
	header += body->getContentType() + "\r\n";
	header += "Content-Length: ";
	header += intToString(body->getContentLength()) + "\r\n\r\n";
}

void	Response::setStatusLine(const StatusCode& statusCode) {
	statusLine += intToString(statusCode.getStatusCode());
	statusLine += " ";
	statusLine += statusCode.getMessage();
	statusLine +=  "\r\n";
}