
#include "Response.hpp"
#include "Config.hpp"

Response::Response(ResponseBody* _body) :
 status(RP_STATUS_LINE), nextIdx(0) , statusLine("HTTP/1.1 "), body(_body){
	setStatusLine(_body->getStatusCode());
	if (_body != NULL){
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

void	Response::setHeader() {
	header.reserve(200);
	header += "Content-Type: ";
	header += body->getContentType() + "\r\n";
	header += "Content-Length: ";
	header += intToString(body->getContentLength()) + "\r\n\r\n";
}

void	Response::setStatusLine(const int& statusCode) {
	statusLine += intToString(statusCode);
	switch (statusCode) {
	case 100:
		statusLine += " Continue\r\n";
		break;
	case 101:
		statusLine += " Switching Protocols\r\n";
		break;
	case 200:
		statusLine += " OK\r\n";
		break;
	case 201:
		statusLine += " Created\r\n";
		break;
	case 202:
		statusLine += " Accepted\r\n";
		break;
	case 203:
		statusLine += " Non-Authoritative Information\r\n";
		break;
	case 204:
		statusLine += " No Content\r\n";
		break;
	case 205:
		statusLine += " Reset Content\r\n";
		break;
	case 206:
		statusLine += " Partial Content\r\n";
		break;
	case 300:
		statusLine += " Multiple Choices\r\n";
		break;
	case 301:
		statusLine += " Moved Permanently\r\n";
		break;
	case 302:
		statusLine += " Moved Temporarily\r\n";
		break;
	case 303:
		statusLine += " See Other\r\n";
		break;
	case 304:
		statusLine += " Not Modified\r\n";
		break;
	case 305:
		statusLine += " Use Proxy\r\n";
		break;
	case 400:
		statusLine += " Bad Request\r\n";
		break;
	case 401:
		statusLine += " Unauthorized\r\n";
		break;
	case 403:
		statusLine += " Forbidden\r\n";
		break;
	case 404:
		statusLine += " Not Found\r\n";
		break;
	case 405:
		statusLine += " Method Not Allowed\r\n";
		break;
	case 406:
		statusLine += " Not Acceptable\r\n";
		break;
	case 407:
		statusLine += " Proxy Authentication Required\r\n";
		break;
	case 408:
		statusLine += " Request Timeout\r\n";
		break;
	case 409:
		statusLine += " Conflict\r\n";
		break;
	case 410:
		statusLine += " Gone\r\n";
		break;
	case 411:
		statusLine += " Length Required\r\n";
		break;
	case 412:
		statusLine += " Precondition Failed\r\n";
		break;
	case 413:
		statusLine += " Request Entity Too Large\r\n";
		break;
	case 414:
		statusLine += " Request-URI Too Long\r\n";
		break;
	case 415:
		statusLine += " Unsupported Media Type\r\n";
		break;
	case 500:
		statusLine += " Internal Server Error\r\n";
		break;
	case 501:
		statusLine += " Not Implemented\r\n";
		break;
	case 502:
		statusLine += " Bad Gateway\r\n";
		break;
	case 503:
		statusLine += " Service Unavailable\r\n";
		break;
	case 504:
		statusLine += " Gateway Timeout\r\n";
	case 505:
		statusLine += " HTTP Version Not Supported\r\n";
		break;
	default:
		break;
	}
}