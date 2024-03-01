#include "HTTPInfo.hpp"

void HTTPInfo::isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion) {
	if (!(method == GET || method == POST || method == DELETE)) {
		throw new exception;
	}
	if (httpVersion != HTTP_VERSION || requestUrl == "") {
		throw new exception;
	}
}

void HTTPInfo::isValidHeaderField(map<string, string>& properties) {
	string contentType = properties[CONTENT_TYPE];
	string contentLength = properties[CONTENT_LENGTH];
	string transferEncoding = properties[TRANSFER_ENCODING];
	string boundary = properties[BOUNDARY];
	string host = properties[HOST];// 호스트 체크 해줘야함

	for (size_t i = 0; i < contentLength.size(); i++) {
		if (!isnumber(contentLength[i])) {
			throw new exception;
		}
	}
	if (!(transferEncoding == "" || transferEncoding == CHUNKED)) {
		throw new exception;
	}
	if (contentType == TEXT_HTML || contentType == TEXT_PLAIN ||
		contentType == IMAGE_PNG || contentType == APPLICATION_OCTET_STREAM) {
		if ((transferEncoding == CHUNKED && contentLength != "") ||
			(transferEncoding != CHUNKED && contentLength == "")) {
			throw new exception;
		}
	} else if (contentType == MULTIPART_FORM_DATA) {
		if (contentLength == "" || transferEncoding != "" || boundary == "") {
			throw new exception;
		}
	} else {
		throw new exception;
	}
}