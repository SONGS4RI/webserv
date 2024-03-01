#pragma once
#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <list>
#include <map>
#include <vector>

#include "Server.hpp"
#include "Client.hpp"

using namespace std;

class SocketManager {
	private:
		static SocketManager* sm;
		list<Server> servers;
		list<Client> clients;

		SocketManager(const vector<Config>& _configs);
	public:
		~SocketManager();
		static SocketManager* getInstance();

		void initServerSocket();
		bool isServerSocket(const int& ident);
		//void disconnectClient(const int& clientIdent);
		//int acceptClient(const int& serverIdent);
		//const Client& getClient(const int& clientIdent);
};

#endif