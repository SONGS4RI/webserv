#include "StatusCode.hpp"

StatusCode::StatusCode() {
	statusCode = 0;
	message = "";
}
StatusCode::StatusCode(const StatusCode& obj) {
	*this = obj;
}

const StatusCode& StatusCode::operator=(const StatusCode& obj) {
	if (this != &obj) {
		statusCode = obj.getStatusCode();
		message = obj.getMessage();
	}
	return (*this);
}

StatusCode::StatusCode(const size_t& statusCode, const string& message) : statusCode(statusCode), message(message) {}

const size_t& StatusCode::getStatusCode() const {
	return statusCode;
}

const string& StatusCode::getMessage() const {
	return message;
}