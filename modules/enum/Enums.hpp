#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

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

enum EConfigType {
	SERVER_CONFIG,
	LOCATION_CONFIG
};

#endif