#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <map>

#include "Config.hpp"
#include "Utils.hpp"
#include "Client.hpp"

using namespace std;

class Client;

class Server {
	private:
		Config serverConfig;
		int	serverSocket;
		map<int, Client> clients;
	public:
		Server(const int& _serverSocket, const Config& config);
		~Server();
		void init(const int& _serverSocket);// config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl()

		map<int, Client>&	getClients();
		int	addClient(int clientSocket);
		const Config& getServerConfig() const;
		const int& getSocketFd() const;
};

#endif