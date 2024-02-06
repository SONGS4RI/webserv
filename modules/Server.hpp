#pragma once

#include <queue>

#include "Config.hpp"

class Server {
	private:
		Config serverConfig;
		int socket;
	public:
		Server(Config config);
		~Server();
		void init();// config를 바탕으로 서버 소켓관련 함수들 socket(), bind(), listen(), fcntl()

		const int& getSocketFd() const;
};
