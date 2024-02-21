#include <iostream>
#include <sstream>
using namespace std;
/**
 * istringstream 으로 잘못읽는 경우 테스트
*/



int main() {
	std::string s = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    istringstream iss(s);
    iss.getline(buf, sizeof(buf));
    string cur(buf, buf + iss.gcount() - 2);
    cout << cur << "\n";
    istringstream tmp(cur);
    string a, b, c;
    tmp.getline(buf, sizeof(buf), ':');
    a = buf;
    cout << a << "\n";
    tmp.getline(buf, sizeof(buf), ';');
    istringstream rest(buf);
    rest >> b >> c;
    cout << b << '\n';
    cout << c << "\n";
}