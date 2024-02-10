#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
using namespace std;

#define BUF_SIZE 1024

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "input error" << endl;
		return 1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	ifstream file(argv[1], ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error opening file!\n";
		return 1;
	}

	char buf[BUF_SIZE];
	file.read(buf, sizeof(buf));
	file.close();
	// 소켓에서 읽기
	istringstream iss(buf);
	while (iss.getline(buf, BUF_SIZE)) {
		if (buf[0] == '\r') {
			cout << "[empty line]";
		}
		cout << buf << " | " << iss.gcount() << endl;
	}

	close(fd);
}