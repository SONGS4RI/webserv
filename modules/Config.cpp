#include "Config.hpp"

Config::Config() {};

Config::Config(Block& globalBlock, Block& serverBlock) {
	type = SERVER_CONFIG;
	clientMaxBodySize = 1000000;
	root = "/";
	index = "";
	autoindexOn = true;

	setByGlobalBlock(globalBlock);
	setByGlobalBlock(globalBlock.blocks[0]);
	setByServerBlock(serverBlock);
	//서버블록의 하위 location 블록들을 set 함.
	for (size_t i = 0; i < serverBlock.blocks.size(); i++) {
		locations[serverBlock.blocks[i].getLocation()] = Config(*this, serverBlock.blocks[i]);
	}
}

Config::Config(Config& serverConfig, Block& locationBlock) {
	type = LOCATION_CONFIG;
	port = serverConfig.getPort();
	serverName = serverConfig.getServerName();
	clientMaxBodySize = serverConfig.getClientMaxBodySize();
	root = serverConfig.getRoot();
	index = serverConfig.getIndex();
	autoindexOn = serverConfig.getAutoindexOn();
	setByLocationBlock(locationBlock);
}

void	Config::setByGlobalBlock(const Block& globalBlock) {
	map<string, vector<string> >::const_iterator	it;

	if ((it = globalBlock.directives.find("client_max_body_size")) != globalBlock.directives.end()) {
		clientMaxBodySize = stoi(it->second[0]);
	}
	if ((it = globalBlock.directives.find("root")) != globalBlock.directives.end()) {
		root = it->second[0];
	}
	if ((it = globalBlock.directives.find("index")) != globalBlock.directives.end()) {
		index = it->second[0];
	}
	if ((it = globalBlock.directives.find("autoindex")) != globalBlock.directives.end()) {
		if (it->second[0] == "on") {
			autoindexOn = true;
		} else if (it->second[0] == "off") {
			autoindexOn = false;
		}
	}
}

void	Config::setByServerBlock(const Block& serverBlock) {
	map<string, vector<string> >::const_iterator	it;
	
	if ((it = serverBlock.directives.find("listen")) != serverBlock.directives.end()) {
		port = stof(it->second[0]);
	} else {
		port = 8080;
	}
	if ((it = serverBlock.directives.find("server_name")) != serverBlock.directives.end()) {
		serverName = it->second[0];
	} else {
		serverName = "webserv.com";
	}
	
	if ((it = serverBlock.directives.find("client_max_body_size")) != serverBlock.directives.end()) {
		clientMaxBodySize = stoi(it->second[0]);
	}
	if ((it = serverBlock.directives.find("root")) != serverBlock.directives.end()) {
		root = it->second[0];
	}
	if ((it = serverBlock.directives.find("index")) != serverBlock.directives.end()) {
		index = it->second[0];
	}
	if ((it = serverBlock.directives.find("autoindex")) != serverBlock.directives.end()) {
		if (it->second[0] == "on") {
			autoindexOn = true;
		} else if (it->second[0] == "off") {
			autoindexOn = false;
		}
	}
}

void	Config::setByLocationBlock(Block& locationBlock) {
	map<string, vector<string> >::const_iterator	it;

	if ((it = locationBlock.directives.find("allow_methods")) != locationBlock.directives.end()) {
		for (size_t i = 0; i < it->second.size(); i++) {
			allowMethods.push_back(it->second[i]);
		}
	} else {
		allowMethods.push_back("GET");
	}
	if ((it = locationBlock.directives.find("return")) != locationBlock.directives.end()) {
		returnRedir = it->second[0];
	} else {
		returnRedir = locationBlock.getLocation();
	}

	if ((it = locationBlock.directives.find("client_max_body_size")) != locationBlock.directives.end()) {
		clientMaxBodySize = stoi(it->second[0]);
	}
	if ((it = locationBlock.directives.find("root")) != locationBlock.directives.end()) {
		root = it->second[0];
	}
	if ((it = locationBlock.directives.find("index")) != locationBlock.directives.end()) {
		index = it->second[0];
	}
	if ((it = locationBlock.directives.find("autoindex")) != locationBlock.directives.end()) {
		if (it->second[0] == "on") {
			autoindexOn = true;
		} else if (it->second[0] == "off") {
			autoindexOn = false;
		}
	}	
}

EConfigType	Config::getType() { return (type);}

map<string, Config>	Config::getLocations() { return (locations);}

int	Config::getPort() { return (port);}

string	Config::getServerName() { return (serverName);}

size_t	Config::getClientMaxBodySize() { return (clientMaxBodySize);}

string	Config::getRoot() { return (root);}

string	Config::getIndex() { return (index);}

bool	Config::getAutoindexOn() { return (autoindexOn);}

string	Config::getReturnRedir() { return (returnRedir);}

vector<string>	Config::getAllowMethods() { return (allowMethods);}

void	Config::printAllInfo(Config& con) {
	if (con.getType() == SERVER_CONFIG) {
		cout << "======== SERVER =========" << endl;
		cout << "type: SERVER_CONFIG" << endl;
		cout << "port: " << port << endl;
		cout << "serverName: " << serverName << endl;
		cout << "clientMaxBodySize: " << clientMaxBodySize << endl;
		cout << "root: " << root << endl;
		cout << "index: " << index << endl;
		cout << "autoindex: " << autoindexOn << endl;
		for (map<string, Config>::iterator it = locations.begin(); it != locations.end(); it++) {
			printAllInfo(it->second);
		}
	} else if (con.getType() == LOCATION_CONFIG) {
		cout << "\t======== LOCATION =========" << endl;
		cout << "\ttype: LOCATION_CONFIG" << endl;
		cout << "\treturn: " << con.returnRedir << endl;
		cout << "\tallowMethods: ";
		for (size_t i = 0; i < con.allowMethods.size(); i++) {
			cout << con.allowMethods[i] << ' ';
		}
		cout << endl;
	}
}