#include "HTTPInfo.hpp"

map<string, vector<string> > HTTPInfo::allowedHeaderField;

void HTTPInfo::init() {
	allowedHeaderField["content-type"].push_back("text/plain");
	allowedHeaderField["content-type"].push_back("text/html");
	allowedHeaderField["content-type"].push_back("application/octet-stream");
	allowedHeaderField["content-type"].push_back("multipart/form-data");
	allowedHeaderField["content-type"].push_back("image/png");
}

bool HTTPInfo::isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion) {
	if (!(method == "GET" || method == "POST" || method == "DELETE")) {
		return false;
	}
	// 유효 url 체크
	if (httpVersion != "HTTP/1.1") {
		return false;
	}
	return true;
}