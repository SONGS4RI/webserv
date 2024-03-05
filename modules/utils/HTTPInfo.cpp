#include "HTTPInfo.hpp"
#include "StatusCode.hpp"
#include "Utils.hpp"

void HTTPInfo::isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion, const Config* serverConfig) {
	vector<string> methods = serverConfig->getAllowMethods();
	if (find(methods.begin(), methods.end(), method) == methods.end()) {
		throw StatusCode(400, BAD_REQUEST);
	}
	if (httpVersion != HTTP_VERSION || requestUrl == "") {
		throw StatusCode(400, BAD_REQUEST);
	}
}

void HTTPInfo::isValidHeaderField(map<string, string>& properties) {
	string contentType = properties[CONTENT_TYPE];
	string contentLength = properties[CONTENT_LENGTH];
	string transferEncoding = properties[TRANSFER_ENCODING];
	string boundary = properties[BOUNDARY];
	string host = properties[HOST];// 호스트 체크 해줘야함

	for (size_t i = 0; i < contentLength.size() && contentType == POST; i++) {// content-length 없을지도??
		if (!isnumber(contentLength[i])) {
			throw StatusCode(400, string(BAD_REQUEST) + ": isValidHeaderField 1");
		}
	}
	if (!(transferEncoding == "" || transferEncoding == CHUNKED)) {
		throw StatusCode(400, string(BAD_REQUEST) + ": isValidHeaderField 2");
	}
	if (contentType == TEXT_HTML || contentType == TEXT_PLAIN ||
		contentType == IMAGE_PNG || contentType == APPLICATION_OCTET_STREAM) {
		if ((transferEncoding == CHUNKED && contentLength != "") ||
			(transferEncoding != CHUNKED && contentLength == "")) {
			throw StatusCode(400, string(BAD_REQUEST) + ": isValidHeaderField 3");
		}
	} else if (contentType == MULTIPART_FORM_DATA) {
		if (contentLength == "" || transferEncoding != "" || boundary == "") {
			throw StatusCode(400, string(BAD_REQUEST) + ": isValidHeaderField 4");
		}
	} else if (properties[METHOD] == POST) {
		throw StatusCode(400, string(BAD_REQUEST) + ": isValidHeaderField 5");
	}
}