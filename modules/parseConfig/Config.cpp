#include "Config.hpp"

Config::Config() {};

Config::Config(Block& globalBlock, Block& serverBlock) {
	type = SERVER_CONFIG;
	clientMaxBodySize = 1000000;
	root = "/";
	index = "";
	autoindexOn = false;

	try
	{
		setByBlock(globalBlock);//global block
		setByBlock(globalBlock.blocks[0]);//http block
		setByBlock(serverBlock);//server block
		/* 서버블록의 하위 location 블록들을 set 함. */
		for (size_t i = 0; i < serverBlock.blocks.size(); i++) {
			locations[serverBlock.blocks[i].getLocation()] = Config(*this, serverBlock.blocks[i]);
		}
	} catch(const char* errmsg) {
		Utils::exitWithErrmsg(string(errmsg));
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
	setByBlock(locationBlock);
}

void	Config::setByBlock(Block& block) {
	map<string, vector<string> >::const_iterator	it;

	/* 모든 블록에 있을 수 있는 지시어 */
	if ((it = block.directives.find("client_max_body_size")) != block.directives.end()) {
		if (isWrongClientMaxBodySize(it->second[0])) {
			throw "Error: client_max_body_size is wrong in configfile";
		}
		clientMaxBodySize = atoi(it->second[0].c_str());
	}
	if ((it = block.directives.find("root")) != block.directives.end()) {
		if (isWrongPath(it->second[0]) == true) {
			cout << it->second[0] << endl;
			throw "Error: root Path is wrong in configfile";
		}
		root = it->second[0];
	}
	if ((it = block.directives.find("index")) != block.directives.end()) {
		index = it->second[0];
	}
	if ((it = block.directives.find("autoindex")) != block.directives.end()) {
		if (it->second[0] == "on") {
			autoindexOn = true;
		} else {
			autoindexOn = false;
		}
	}
	if (block.type == "server") {//서버블록에만 있는 지시어
		if ((it = block.directives.find("listen")) != block.directives.end()) {
			if (isWrongPort(it->second[0]) == true) {
				throw "Error: wrong port in configfile";
			}
			port = atoi(it->second[0].c_str());
		} else {
			port = 8080;
		}
		if ((it = block.directives.find("server_name")) != block.directives.end()) {
			serverName = it->second[0];
		} else {
			serverName = "webserv.com";
		}
	} else if (block.type == "location") {//로케이션블록에만 있는 지시어
		if ((it = block.directives.find("allow_methods")) != block.directives.end()) {
			for (size_t i = 0; i < it->second.size(); i++) {
				if (isWrongAllowMethod(it->second[i]) == true) {
					throw "Error: allow_methods is wrong in configfile";
				}
				allowMethods.push_back(it->second[i]);
			}
		}
		if ((it = block.directives.find("return")) != block.directives.end()) {
			if (isWrongPath(it->second[0]) == true) {
				throw "Error: return Path is wrong in configfile";
			}
			returnRedir = it->second[0];
		} else {
			returnRedir = block.getLocation();
		}
		if ((it = block.directives.find("alias")) != block.directives.end()) {
			if (isWrongPath(it->second[0]) == true) {
				throw "Error: alias Path is wrong in configfile";
			}
			alias = it->second[0];
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

//Config 내용 프린트 해보는 함수
void	Config::printAllInfo() {
	if (this->getType() == SERVER_CONFIG) {
		cout << "======== SERVER =========" << endl;
		cout << "type: SERVER_CONFIG" << endl;
		cout << "port: " << port << endl;
		cout << "serverName: " << serverName << endl;
		cout << "clientMaxBodySize: " << clientMaxBodySize << endl;
		cout << "root: " << root << endl;
		cout << "index: " << index << endl;
		cout << "autoindex: " << autoindexOn << endl;
		for (map<string, Config>::iterator it = locations.begin(); it != locations.end(); it++) {
			it->second.printAllInfo();
		}
	} else if (this->getType() == LOCATION_CONFIG) {
		cout << "\t======== LOCATION =========" << endl;
		cout << "\ttype: LOCATION_CONFIG" << endl;
		cout << "\treturn: " << this->returnRedir << endl;
		cout << "\tallowMethods: ";
		for (size_t i = 0; i < this->allowMethods.size(); i++) {
			cout << this->allowMethods[i] << ' ';
		}
		cout << endl;
		cout << "\talias: " << this->alias << endl;
	}
}

//block의 string port에 문제있는지 확인
bool	Config::isWrongPort(const string& portString) const {
	if (portString.size() >= 2 && portString[0] == '0') {//080 이렇게 쓰는거 불허
		return (true);
	}
	if (portString.size() > 5) {//port 최대치는 65535
		return (true);
	}
	for (size_t i = 0; i < portString.size(); i++) {//포트는 양의 정수만 가짐
		if (isdigit(portString[i]) == false) {
			return (true);
		}
	}
	int	port_ = atoi(portString.c_str());//port 최대치는 65535
	if (port_ > 65535) {
		return (true);
	}
	return (false);
}
//block의 string clientMaxBodySize에 문제 있는지 확인
bool	Config::isWrongClientMaxBodySize(const string& CMBS_String) const {
	if (CMBS_String.size() >= 2 && CMBS_String[0] == '0') {//01000 이렇게 쓰는거 불허
		return (true);
	}
	if (CMBS_String.size() > 10) {//최대치는 2147483647
		return (true);
	}
	for (size_t i = 0; i < CMBS_String.size(); i++) {//양의 정수만 가짐
		if (isdigit(CMBS_String[i]) == false) {
			return (true);
		}
	}
	if (CMBS_String.size() == 10 && CMBS_String[0] >= '3') {//최대치는 2147483647
		return (true);
	}
	int	CMBS = atoi(CMBS_String.c_str());
	if (CMBS < 0) {//오버플로우 일어났으면 2147483647 보다 큰 경우.
		return (true);
	}
	return (false);
}
//allow_method GET POST DELETE만 허용함.
bool	Config::isWrongAllowMethod(const string& allowMethods_) const {
	if (allowMethods_ != "GET" && allowMethods_ != "POST" && allowMethods_ != "DELETE") {
		return (true);
	}
	return (false);
}

bool	Config::isWrongPath(const string& path) {
	if (path[0] != '/') {
		return (true);
	}
	return (false);
}