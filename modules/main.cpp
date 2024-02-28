#include "ParseConfig.hpp"
#include "Config.hpp"
#include "Response.hpp"
#include "ResponseBody.hpp"
#include "fortest.hpp"
//#include "EventManager.hpp"
//#include "SocketManager.hpp"
//#include "Server.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc != 2) {
		exitWithErrmsg(string("Error: ") + argv[0] + " [ConfigFile]");
	}
//	SocketManager* sm = SocketManager::getInstance();
//	EventManager* em = EventManager::getInstance();
	vector<Config> serverConfigs;
	try {
		ParseConfig	parsedConfigFile(argv[1]);
		serverConfigs = parsedConfigFile.getServerConfigs();
	} catch(const char* errstr) {
		exitWithErrmsg(errstr);
	}
	string	tmpType("text/plain");
	string	tmpBody = fileToString("configuration_example/index.html");
	ResponseBody*	tmpRBody = new ResponseBody(200, tmpType, tmpBody);
	Response	response(tmpRBody);
	cout << response.isDone() << endl;
	response.writeToSocket(1);
	cout << response.isDone() << endl;
	response.writeToSocket(1);
	
	//response.printAllInfo();
	// for (vector<Config>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); it++) {
	// 	it->printAllInfo();
	// }
/*	
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
	*/
}