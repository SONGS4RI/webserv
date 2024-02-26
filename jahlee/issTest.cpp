#include <iostream>
#include <sstream>
using namespace std;
/**
 * istringstream 으로 잘못읽는 경우 테스트
*/



int main() {
	// std::string s = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	std::string s = "12345";
    istringstream iss(s);
    char buf[BUFSIZ];
    iss.read(buf, 6);
    cout << buf << "\n";
    if (iss.eof()) {
        cout << "eof\n";
    }
}