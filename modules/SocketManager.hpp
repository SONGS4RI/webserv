#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <list>
#include <map>

#include "Server.hpp"
#include "Client.hpp"

using namespace std;

class SocketManager {
	private:
		static SocketManager* sm;
		list<Server> servers;
		list<Client> clients;

		SocketManager(/* args */);
	public:
		~SocketManager();// 동적할당 해제해야함
		static SocketManager* getInstance();

		void initServerSocket(const vector<Config>& serverConfigs);
		bool isServerSocket(const int& ident);
		void disconnectClient(const int& clientIdent);
		int acceptClient(const int& serverIdent);
		const Client& getClient(const int& clientIdent);
};

#endif