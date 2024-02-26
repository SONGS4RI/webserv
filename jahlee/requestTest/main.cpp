#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "RequestTest.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		cerr << "arg err\n";
	}
	int fd = open(argv[1], O_RDONLY);
	RequestTest req(0);
	// req.parseRequest(fd);
	try {
		while (req.getStatus() != PARSE_DONE) {
			cout << "==================\n";
			req.parseRequest(fd);
		}
	}
	catch(const std::exception* e) {
		cout << "ERROR\n";
	}
	req.toString();
	close(fd);
}