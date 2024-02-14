#include <iostream>
#include <sstream>
using namespace std;
/**
 * istringstream 으로 잘못읽는 경우 테스트
*/
int main() {
	string s = "01234\r\n456\r\n789\r\n";
	char buf[1024];
	istringstream iss(s, s.size());
	while (!iss.eof()) {
		iss.getline(buf, sizeof(buf));
		cout << buf << ": " << iss.gcount() << "\n";
		if (iss.fail()) {
			cout << "fail" << "\n";
		}
	}
	// cout << buf << "\n";
	// if (iss.fail()) {
	// 	cout << "fail\n";
	// }
	// if (iss.eof()) {
	// 	cout << "eof\n";
	// }
}