#include "Config.hpp"
#include "EventManager.hpp"
#include "SocketManager.hpp"
#include "Server.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		return (1);
	}
	// parse config
	SocketManager* sm = SocketManager::getInstance();
	EventManager* em = EventManager::getInstance();
	vector<Config> serverConfigs = Config::parse(argv[1]);
	sm->initServerSocket(serverConfigs);
	int newEvents;

	while (1) {
		newEvents = em->detectEvent();
		for (int i=0; i<newEvents; i++) {
			try {
				em->handleEvent(i);
			} catch(const std::exception& e) {
				std::cerr << e.what() << '\n';
			}
		}
	}
	delete sm;
	delete em;
	return (0);
}