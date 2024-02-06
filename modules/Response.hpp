#pragma once

enum EResponseStatus {
	WAIT,
	PROGRESS,
	DONE
};

class Response {
	

	private:
		EResponseStatus status;
	public:
		Response(/* args */);
		~Response();
};