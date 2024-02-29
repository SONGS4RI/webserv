#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include "Server.hpp"

Server::Server(const Config& _config) : serverConfig(_config) {}

const Config& Server::getServerConfig() const { return (serverConfig);}

const int& Server::getSocketFd() const { return (serverSocket);}

void Server::init() {
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		exitWithErrmsg("Error: socket()");
	}
	struct sockaddr_in	serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverConfig.getPort());
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		exitWithErrmsg("Error: bind()");
	}
	if (listen(serverSocket, 10) == -1) {
		exitWithErrmsg("Error: listen()");
	}
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		exitWithErrmsg("Error: fcntl()");
	}
}