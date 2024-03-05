#include <cstring>
#include <netinet/in.h>
#include <fcntl.h>
#include "Server.hpp"

Server::Server(const int& _serverSocket, const Config& _config) : serverConfig(_config)
{
	init(_serverSocket);
	serverSocket = _serverSocket;
}

Server::~Server() {
	close(serverSocket);
	for (map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->second != NULL) {
			delete it->second;
			it->second = NULL;
		}
	}
}
void	Server::addClient(int clientSocket) {
	Client*	client = new Client(clientSocket, *this);

	clients.insert(make_pair(clientSocket, client));
}
const Config& Server::getServerConfig() const { return (serverConfig);}

const int& Server::getSocketFd() const { return (serverSocket);}

map<int, Client*>&	Server::getClients() { return (clients);}

void Server::init(const int& _serverSocket) {
	struct sockaddr_in	serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverConfig.getPort());
	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		Utils::exitWithErrmsg("Error: bind()");
	}
	if (listen(_serverSocket, 10) == -1) {
		Utils::exitWithErrmsg("Error: listen()");
	}
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		Utils::exitWithErrmsg("Error: fcntl()");
	}
}