#pragma once

enum EStatus {
	START_LINE,
	HEADER,
	BODY,
	PARSE_DONE,
	ERROR
};

enum EType {
	DEFAULT,
	CHUNKED,
	BINARY
};

class Request {
	private:
		EStatus status;
		EType bodyType;
	public:
		Request(/* args */);
		~Request();

};