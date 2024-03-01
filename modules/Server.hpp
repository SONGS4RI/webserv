#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>

#include "Config.hpp"
#include "Utils.hpp"

class Server {
	private:
		Config serverConfig;
		int serverSocket;
	public:
		Server(const Config& config);
		~Server();
		void init();// config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl()

		const Config& getServerConfig() const;
		const int& getSocketFd() const;
};

#endif