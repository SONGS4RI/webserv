#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <map>
#include <vector>

#include "Server.hpp"
#include "Client.hpp"
#include "Utils.hpp"

using namespace std;

class SocketManager {
	private:
		static SocketManager* sm;
		static vector<Config> configs;
		map<int, Server> servers;

		SocketManager();
	public:
		~SocketManager();
		static void	setConfigs(const vector<Config>& _configs);
		static SocketManager* getInstance();

		void initServers();
		bool isServerSocket(const int& ident);
		void disconnectClient(const int& clientIdent);
		int acceptClient(const int& serverIdent);
		const Client* getClient(const int& clientIdent);
};

#endif