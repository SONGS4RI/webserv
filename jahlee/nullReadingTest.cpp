#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

/*
	파일 읽을때 중간에 널이 들어간거 테스트
*/

int main() {
	vector<char> v = {'0', '1', '2', '\0', '3', '4'};
	string s(v.begin(), v.end());
	int fd = open("nullReadingTestResult", O_WRONLY | O_TRUNC | O_CREAT, 0644);
	write(fd, s.c_str(), s.size());
	close(fd);

	char buf[1024];
	fd = open("nullReadingTestResult", O_RDONLY, ios::binary);
	int n = read(fd, buf, 6);
	cout << n << "\n";
	string tmp(n, '\0');
	for (int i = 0; i < n; i++) {
		tmp[i] = buf[i];
	}
	istringstream iss(tmp);
	iss.read(buf, n);
	cout << buf << "\n";
	cout << (iss.eof() ? "true" : "false") << "\n";

	n = read(fd, buf, 6);
	cout << n << "\n";
	for (int i = 0; i < n; i++) {
		tmp[i] = buf[i];
	}
	iss.str(tmp);
	iss.read(buf, n);
	cout << buf << "\n";
	cout << (iss.eof() ? "true" : "false") << "\n";

	close(fd);
}