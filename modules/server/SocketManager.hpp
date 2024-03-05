#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <map>
#include <vector>
#include <fcntl.h>


#include "Server.hpp"
#include "Client.hpp"
#include "../utils/Utils.hpp"

using namespace std;

class SocketManager {
	private:
		static SocketManager* sm;
		map<int, Server*> servers;
	public:
		SocketManager();
		~SocketManager();
		void	init(vector<Config>& configs);
		static SocketManager* getInstance();

		bool isServerSocket(const int& ident);
		void	addClient(int serverSocket, int clientSocket);
		void disconnectClient(const int& clientIdent);
		int acceptClient(const int& serverIdent);
		Client* getClient(const int& clientIdent);
		const Config* getServerConfig(const int& clientIdent);
		map<int, Server*>& getServers();
};

#endif