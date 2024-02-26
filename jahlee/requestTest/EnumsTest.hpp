#pragma once
#ifndef ENUMS_TEST_HPP
#define ENUMS_TEST_HPP

enum EResponseStatus {
	WAIT,
	PROGRESS,
	DONE
};

enum ERequestStatus {
	START_LINE,
	HEADER,
	BODY,
	PARSE_DONE
};

enum EChunkedStatus {
	LENGTH,
	DATA,
	CRLF
};

#endif