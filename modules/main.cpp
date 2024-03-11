#include "./parseConfig/ParseConfig.hpp"
#include "./parseConfig/Config.hpp"
#include "./response/Response.hpp"
#include "./response/ResponseBody.hpp"
#include "./utils/StatusCode.hpp"
#include "./server/EventManager.hpp"
#include "./server/SocketManager.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		Utils::exitWithErrmsg(string("Error: ") + argv[0] + " [ConfigFile]");
	}
	SocketManager* sm = SocketManager::getInstance();
	EventManager* em = EventManager::getInstance();
	try {
		ParseConfig	parsedConfigFile(argv[1]);
		sm->init(parsedConfigFile.getServerConfigs());
		vector<Config>::iterator it = parsedConfigFile.getServerConfigs().begin();
		for (; it != parsedConfigFile.getServerConfigs().end(); it++) {
			it->printAllInfo();
		}
	} catch(const char* errstr) {
		Utils::exitWithErrmsg(errstr);
	}
	for (map<int, Server*>::iterator sit = sm->getServers().begin(); sit != sm->getServers().end(); sit++) {
		em->changeEvent(sit->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	Utils::log("Server started", GREEN);
	while (1) {
		int	newEvents = em->detectEvent();
		Utils::log(Utils::intToString(newEvents) + "개 Event 발생", GREEN);
		for (int i = 0; i < newEvents; ++i) {
			em->handleEvent(i);
		}
	}
	delete sm;
	delete em;
}