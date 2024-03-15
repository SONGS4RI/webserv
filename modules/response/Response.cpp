#include <cmath>
#include "Response.hpp"
#include "../config/Config.hpp"

Response::Response(ResponseBody* _body) : status(RP_STATUS_LINE), nextIdx(0) , statusLine("HTTP/1.1 "), body(_body) {
	setStatusLine(_body->getStatusCode());
	setHeader();
}

void	Response::writeToSocket(int fd) {
	size_t	writeAmount;
	size_t	writtenLength = 0;
	
	while (BUF_SIZE > writtenLength && status != RP_DONE) {
		switch (status) {
			case RP_STATUS_LINE:
				writeAmount = min(statusLine.size() - nextIdx , BUF_SIZE - writtenLength);
				write(fd, &statusLine[nextIdx], writeAmount);
				nextIdx += writeAmount;
				writtenLength += writeAmount;
				if (nextIdx == statusLine.size()) {
					status = RP_HEADER;
					nextIdx = 0;
				}
				break;
			case RP_HEADER:
				writeAmount = min(header.size() - nextIdx , BUF_SIZE - writtenLength);
				write(fd, &header[nextIdx], writeAmount);
				nextIdx += writeAmount;
				writtenLength += writeAmount;
				if (nextIdx == header.size()) {
					status = RP_BODY;
					nextIdx = 0;
				}
				break;
			case RP_BODY:
				if (body == NULL) {
					status = RP_DONE;
					return ;
				}
				writeAmount = min(body->getBody().size() - nextIdx , BUF_SIZE - writtenLength);
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
				return ;
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
	if (body != NULL) {
		delete body;
		body = NULL;
	}
}

void	Response::setHeader() {
	header.reserve(200);
	if (body->getContentType() != "") {
		header += "Content-Type: ";
		header += body->getContentType() + "\r\n";
	}
	if (body->getContentLength() != 0) {
		header += "Content-Length: ";
		header += Utils::intToString(body->getContentLength()) + "\r\n";
	}
	if (body->getLocation() != "") {
		header += "Location: ";
		header += body->getLocation() + "\r\n";
	}
	header += "\r\n";
}

void	Response::setStatusLine(const StatusCode& statusCode) {
	statusLine += Utils::intToString(statusCode.getStatusCode());
	statusLine += " ";
	statusLine += statusCode.getMessage();
	statusLine +=  "\r\n";
}