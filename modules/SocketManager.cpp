#include "SocketManager.hpp"

SocketManager::SocketManager(const vector<Config>& _configs) {
	for (size_t i = 0; i < _configs.size(); ++i) {
		servers.push_back(Server(_configs[i]));
	}
}

SocketManager* SocketManager::getInstance() {
	if (sm == NULL) {
		sm = new SocketManager();
	}
	return (sm);
}

SocketManager::~SocketManager() {}
/* server의 config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl() */
void SocketManager::initServerSocket() {
	for (list<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		it->init();
	}
}

bool SocketManager::isServerSocket(const int& ident) {
	for (list<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		if (it->getSocketFd() == ident) {
			return (true);
		}
	}
	return (false);
}