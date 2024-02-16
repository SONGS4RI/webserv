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

enum EMothod {
	GET,
	POST,
	DELETE
};

#endif