#ifndef STATUSCODE_HPP
# define STATUSCODE_HPP

#include <string>
using namespace std;

#define OK "OK"
#define CREATED "CREATED"
#define NO_CONTENT "NO CONTENT"
#define BAD_REQUEST "BAD REQUEST"
#define NOT_FOUND "NOT FOUND"
#define FORBIDDEN "FORBIDDEN"
#define INTERVER_SERVER_ERROR "INTERVER SERVER ERROR"

class StatusCode {
	private:
		size_t statusCode;
		string message;
	public:
		StatusCode() {
			statusCode = 0;
			message = "";
		}
		StatusCode(const StatusCode& obj) {
			*this = obj;
		}

		const StatusCode& operator=(const StatusCode& obj) {
			if (this != &obj) {
				statusCode = obj.getStatusCode();
				message = obj.getMessage();
			}
		}

		StatusCode(const size_t& statusCode, const string& message);

		const size_t& getStatusCode() const {
			return statusCode;
		}

		const string& getMessage() const {
			return message;
		}
};

#endif
