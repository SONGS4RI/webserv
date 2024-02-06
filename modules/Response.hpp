#pragma once

enum EStatus {
	WAIT,
	PROGRESS,
	DONE
};

class Response {
	private:
		EStatus status;
	public:
		Response(/* args */);
		~Response();
};