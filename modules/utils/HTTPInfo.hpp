#pragma once
#ifndef HTTPINFO_HPP
#define HTTPINFO_HPP

#include <map>
#include <iostream>

using namespace std;

#define METHOD "method"
#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
#define REQUEST_URL "requestUrl"
#define HTTP_VERSION "HTTP/1.1"
#define HOST "host"
#define CONTENT_TYPE "content-type"
#define TEXT_PLAIN "text/plain"
#define TEXT_HTML "text/html"
#define APPLICATION_OCTET_STREAM "application/octet-stream"
#define MULTIPART_FORM_DATA "multipart/form-data"
#define IMAGE_PNG "image/png"
#define CONTENT_LENGTH "content-length"
#define BOUNDARY "boundary"
#define TRANSFER_ENCODING "transfer-encoding"
#define CHUNKED "chunked"

class HTTPInfo {
	private :

	public :
		static void isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion);
		static void isValidHeaderField(map<string, string>& properties);
};

#endif