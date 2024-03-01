#include "SocketManager.hpp"

SocketManager*	SocketManager::sm = NULL;

vector<Config>	SocketManager::configs;

SocketManager::SocketManager() {
	initServers();
}

SocketManager* SocketManager::getInstance() {
	if (sm == NULL) {
		sm = new SocketManager();
	}
	return (sm);
}

void	SocketManager::setConfigs(const vector<Config>& _configs) {
	configs = _configs;
}

SocketManager::~SocketManager() {}

/* server의 config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl() */
void SocketManager::initServers() {
	int	serverSocket;
	for (size_t i = 0; i < configs.size(); ++i) {
		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocket == -1) {
			exitWithErrmsg("Error: socket()");
		}
		Server	server(serverSocket, configs[i]);
		
		servers.insert(make_pair(serverSocket, server));
	}
}

/* ident를 받아 servers에 일치하는 소켓 있는지 확인하는 함수 */
bool SocketManager::isServerSocket(const int& ident) {
	if (servers.find(ident) == servers.end()) {
		return (false);
	}
	return (true);
}

/* ident를 받아 클라이언트를 제거하는 함수. close는 Client 클래스의 소멸자로 한다. */
void SocketManager::disconnectClient(const int& clientIdent) {
	for (map<int, Server>::iterator sit = servers.begin(); sit != servers.end(); sit++) {
		map<int, Client>::iterator cit = sit->second.getClients().find(clientIdent);
		if (cit != sit->second.getClients().end()) {
			sit->second.getClients().erase(cit);
		}
	}
}

/* 서버소켓에 read가 감지되면(새 클라이언트가 접속 시) 실행될 함수.
	클라이언트로의 접속을 수락하고 새 Client 객체를 만들어 list에 추가한다. 그리고 추가된 클라이언트 소켓 리턴한다.*/
int SocketManager::acceptClient(const int& serverIdent) {
	int	clientSocket;

	clientSocket = accept(serverIdent, NULL, NULL);
	if (clientSocket == -1) {
		/* 에러 처리 */
	}
	map<int, Server>::iterator sit = servers.find(serverIdent);
	if (sit == servers.end()) {
		/* 에러 처리 */
	}
	sit->second.addClient(clientSocket);
	return (clientSocket);
}

const Client* SocketManager::getClient(const int& clientIdent) {
	for (map<int, Server>::iterator sit = servers.begin(); sit != servers.end(); sit++) {
		map<int, Client>::iterator cit = sit->second.getClients().find(clientIdent);

		if (cit != sit->second.getClients().end()) {
			return (&(cit->second));
		}
	}
	return (NULL);
}