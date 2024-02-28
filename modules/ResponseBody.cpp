#include "ResponseBody.hpp"

ResponseBody::ResponseBody(size_t code, string _type, string _body) {
	statusCode = code;
	contentType = _type;
	contentLength = _body.size();
	body = _body;
}

const size_t& ResponseBody::getStatusCode() const {
	return (statusCode);
}

const string& ResponseBody::getContentType() const {
	return (contentType);
}

const size_t& ResponseBody::getContentLength() const {
	return (contentLength);
}

const string& ResponseBody::getBody() const {
	return (body);
}