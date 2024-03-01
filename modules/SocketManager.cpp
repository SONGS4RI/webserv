#include "SocketManager.hpp"

SocketManager::SocketManager(const vector<Config>& _configs) {
	for (size_t i = 0; i < _configs.size(); ++i) {
		servers.push_back(Server(_configs[i]));
	}
}

SocketManager* SocketManager::getInstance() {
	if (sm == NULL) {
		sm = new SocketManager(configs);
	}
	return (sm);
}

void	SocketManager::setConfigs(const vector<Config>& _configs) {
	configs = _configs;
}

SocketManager::~SocketManager() {}

/* server의 config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl() */
void SocketManager::initServerSocket() {
	for (list<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		it->init();
	}
}

/* ident를 받아 서버 list에 일치하는 소켓 있는지 확인하는 함수 */
bool SocketManager::isServerSocket(const int& ident) {
	for (list<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		if (it->getSocketFd() == ident) {
			return (true);
		}
	}
	return (false);
}

/* ident를 받아 클라이언트를 제거하는 함수. close는 Client 클래스의 소멸자로 한다. */
void SocketManager::disconnectClient(const int& clientIdent) {
	for (list<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getSocketFd() == clientIdent) {
			clients.erase(it);
			break ;
		}
	}
}

/* 서버소켓에 read가 감지되면(새 클라이언트가 접속 시) 실행될 함수.
	클라이언트로의 접속을 수락하고 새 Client 객체를 만들어 list에 추가한다. */
int SocketManager::acceptClient(const int& serverIdent) {
	int	clientSocket;

	clientSocket = accept(serverIdent, NULL, NULL);
	if (clientSocket == -1) {
		/* 에러 처리 */
	}
	clients.push_back(Client(clientSocket));
	return (clientSocket);
}

const Client* SocketManager::getClient(const int& clientIdent) {
	for (list<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getSocketFd() == clientIdent) {
			return (&(*it));
		}
	}
	/* 클라이언트가 없을 경우 throw 해줘야 할듯!!! */
	return (NULL);
}