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
	int n = read(fd, buf, sizeof(buf));
	string tmp(n, '\0');
	for (int i = 0; i < n; i++) {
		tmp[i] = buf[i];
	}
	istringstream iss(tmp);
	while (!iss.eof()) {
		iss >> tmp;
		cout << tmp << "\n";
	}
	string leftOver(tmp);
	cout << leftOver << "\n";
	cout << leftOver.size() << "\n";
	close(fd);
}