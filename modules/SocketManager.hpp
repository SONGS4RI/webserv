#pragma once

#include <vector>
#include <map>

#include "Server.hpp"
#include "Client.hpp"

using namespace std;

class SocketManager {
	private:
		static SocketManager* sm;
		vector<Server> servers;
		vector<Client> clients;

		SocketManager(/* args */);
	public:
		~SocketManager();// 동적할당 해제해야함
		static SocketManager* getInstance();

		void initServerSocket(const vector<Config>& serverConfigs);
		bool isServerSocket(const int& ident);
		void disconnect_client(const int& clientIdent);
		Client& getClient(const int& clientIdent);
};
