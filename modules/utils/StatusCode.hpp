#ifndef STATUSCODE_HPP
# define STATUSCODE_HPP

#include <string>
using namespace std;

#define OK "OK"
#define CREATED "CREATED"
#define NO_CONTENT "NO CONTENT"
#define BAD_REQUEST "BAD REQUEST"
#define SEE_OTHERS "SEE OTHERS"
#define NOT_FOUND "NOT FOUND"
#define FORBIDDEN "FORBIDDEN"
#define INTERNAL_SERVER_ERROR "INTERNAL SERVER ERROR"

class StatusCode {
	private:
		size_t statusCode;
		string message;
	public:
		StatusCode();
		StatusCode(const StatusCode& obj);

		const StatusCode& operator=(const StatusCode& obj);

		StatusCode(const size_t& statusCode, const string& message);

		const size_t& getStatusCode() const;

		const string& getMessage() const;
};

#endif
