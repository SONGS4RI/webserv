#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <list>
#include <queue>

#include "Config.hpp"
#include "Utils.hpp"
#include "Client.hpp"

class Client;

class Server {
	private:
		Config serverConfig;
		int serverSocket;
		list<Client> clients;
	public:
		Server(const Config& config);
		~Server();
		void init();// config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl()

		list<Client>&	getClients();
		const Config& getServerConfig() const;
		const int& getSocketFd() const;
};

#endif