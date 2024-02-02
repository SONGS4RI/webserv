#include <Config.hpp>
#include <EventManager.hpp>
#include <SocketManager.hpp>
#include <Server.hpp>

#include <map>

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		return (1);
	}

	SocketManager* sm = SocketManager::getInstance();
	EventManager* em = EventManager::getInstance();

	try {
		vector<Config> serverConfigs = Config::parse(argv[1]);

		sm->initServerSocket(serverConfigs);

		int newEvents;

		while (1) {
			newEvents = em->detectEvent();
			for (int i=0; i<newEvents; i++) {
				em->handleEvent(i);
			}
		}
	} catch (exception& e) {
		cout << e.what() << endl;
		delete sm;
		delete em;
		return (1);
	}
	// 싱글톤 객체들 삭제
	delete sm;
	delete em;
	return (0);
}