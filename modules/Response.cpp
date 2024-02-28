
#include "Response.hpp"

Response::Response(int statusCode, const string& _contentsType, const string& _body) :
	statusLine("HTTP/1.1 "), header(""), body(_body), contentType(_contentsType), contentLength(_body.size()){
	setStatusLine(statusCode);
	setHeader();
}

void	Response::writeToSocket(int fd) {
	write(fd, statusLine.c_str(), statusLine.size());
	write(fd, header.c_str(), header.size());
	if (body.size() > 0) {
		write(fd, "\r\n", 2);
		write(fd, body.c_str(), body.size());
	}
}

Response::~Response(){
}

void	Response::setHeader() {
	header.resize(100);
	header += "Content-Type: ";
	header += contentType + "\r\n";
	header += "Content-Length: ";
	header += intToString(contentLength) + "\r\n";
}

void	Response::setStatusLine(int& statusCode) {
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

void	Response::printAllInfo() {
	cout << statusLine;
	cout << header;
	cout << body << endl;
}