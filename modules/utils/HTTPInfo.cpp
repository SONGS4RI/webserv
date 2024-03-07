#include "HTTPInfo.hpp"
#include "StatusCode.hpp"
#include "Utils.hpp"
#include "unistd.h"

char currentPath[FILENAME_MAX];

string HTTPInfo::root = string(getcwd(currentPath, sizeof(currentPath))) + "/../root/" /* + config root*/;
string HTTPInfo::defaultRoot = string(getcwd(currentPath, sizeof(currentPath))) + "/../root/";

void HTTPInfo::isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion, const Config* serverConfig) {
	vector<string> methods = serverConfig->getAllowMethods();
	if (find(methods.begin(), methods.end(), method) == methods.end()) {
		throw StatusCode(400, string(BAD_REQUEST) + ": Not Allowed Method");
	}
	if (httpVersion != HTTP_VERSION || requestUrl == "") {
		throw StatusCode(400, string(BAD_REQUEST) + ": invalid format");
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

string HTTPInfo::convertToMIME(const string& contentType) {
	if (contentType == "html") {
		return TEXT_HTML;
	} else if (contentType == "png") {
		return IMAGE_PNG;
	} else if (contentType == "" || contentType == "txt") {
		return TEXT_PLAIN;
	} else if (contentType == ".bin") {
		return APPLICATION_OCTET_STREAM;
	}
	return "";
}