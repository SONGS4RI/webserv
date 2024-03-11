#include "Config.hpp"

Config::Config() {};

Config::Config(Block& globalBlock, Block& serverBlock) {
	type = SERVER_CONFIG;
	port = 8080;
	serverName = "webserv.com";
	root = "/root";
	index = "";
	clientMaxBodySize = 1000000;
	defaultErrorPage = "error.html";
	autoindexOn = true;
	alias = "";
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
	root = serverConfig.getRoot();
	index = serverConfig.getIndex();
	clientMaxBodySize = serverConfig.getClientMaxBodySize();
	allowMethods = serverConfig.getAllowMethods();
	defaultErrorPage = serverConfig.getDefaultErrorPage();
	autoindexOn = serverConfig.getAutoindexOn();
	setByBlock(locationBlock);
}

void	Config::setByBlock(Block& block) {
	map<string, vector<string> >::const_iterator	it;

	/* 모든 블록에 있을 수 있는 지시어 */
	if ((it = block.directives.find("error_page")) != block.directives.end()) {
		defaultErrorPage = it->second[0];
	}
	if ((it = block.directives.find("client_max_body_size")) != block.directives.end()) {
		if (isWrongClientMaxBodySize(it->second[0])) {
			throw "Error: client_max_body_size is wrong in configfile";
		}
		clientMaxBodySize = atoi(it->second[0].c_str());
	}
	if ((it = block.directives.find("index")) != block.directives.end()) {
		index = it->second[0];
	}
	if ((it = block.directives.find("allow_methods")) != block.directives.end()) {
		vector<string>	allowMethodsBlock = it->second;
		for (size_t i = 0; i < allowMethodsBlock.size(); i++) {
			if (isWrongAllowMethod(allowMethodsBlock[i]) == true) {
				throw "Error: allow_methods is wrong in configfile";
			}
			if (find(allowMethods.begin(), allowMethods.end(), allowMethodsBlock[i]) == allowMethods.end()) {
				allowMethods.push_back(it->second[i]);
			}
		}
	}
	if ((it = block.directives.find("root")) != block.directives.end()) {
		if (isWrongPath(it->second[0]) == true) {
			cout << it->second[0] << endl;
			throw "Error: root Path is wrong in configfile";
		}
		root = "/root" + (it->second[0] == "/" ? "" : it->second[0]);
		if (root.back() == '/') {
			throw "Error: root Path is wrong in configfile";
		}
	}
	if (block.type == "server") {//서버블록에만 있는 지시어
		if ((it = block.directives.find("autoindex")) != block.directives.end()) {
			if (it->second[0] == "off") {
				autoindexOn = false;
			}
		}
		if ((it = block.directives.find("listen")) != block.directives.end()) {
			if (isWrongPort(it->second[0]) == true) {
				throw "Error: wrong port in configfile";
			}
			port = atoi(it->second[0].c_str());
		}
		if ((it = block.directives.find("server_name")) != block.directives.end()) {
			serverName = it->second[0];
		}
	} else if (block.type == "location") {//로케이션블록에만 있는 지시어
		if ((it = block.directives.find("alias")) != block.directives.end()) {
			if (isWrongPath(it->second[0]) == true) {
				throw "Error: alias Path is wrong in configfile";
			}
			alias = it->second[0];
		}
	}
}

const EConfigType	Config::getType() const { return (type);}

const int&	Config::getPort() const { return (port);}

const string&	Config::getRoot() const { return (root);}

string	Config::getServerName() { return (serverName);}

const size_t& Config::getClientMaxBodySize() const { return (clientMaxBodySize);}
const size_t& Config::getClientMaxBodySize(string loc) const {
	map<string, Config>::const_iterator it = locations.find(loc);
	if (it != locations.end()) {
		return (it->second.getClientMaxBodySize());
	}
	return (clientMaxBodySize);
}

const string&	Config::getDefaultErrorPage() const { return (defaultErrorPage);}
const string&	Config::getDefaultErrorPage(string loc) const {
	map<string, Config>::const_iterator it = locations.find(loc);
	if (it != locations.end()) {
		return (it->second.getDefaultErrorPage());
	}
	return (defaultErrorPage);
}

const string	Config::getIndex() const { return (index);}
const string	Config::getIndex(string loc) const {
	map<string, Config>::const_iterator it = locations.find(loc);
	if (it != locations.end()) {
		return (it->second.getIndex());
	}
	return ("");
}

const bool&	Config::getAutoindexOn() const { return (autoindexOn);}
const bool&	Config::getAutoindexOn(string loc) const {
	map<string, Config>::const_iterator it = locations.find(loc);
	if (it != locations.end()) {
		return (it->second.getAutoindexOn());
	}
	return (autoindexOn);
}

const vector<string>&	Config::getAllowMethods() const { return (allowMethods);}
const vector<string>&	Config::getAllowMethods(string loc) const {
	map<string, Config>::const_iterator it = locations.find(loc);
	if (it != locations.end()) {
		return (it->second.getAllowMethods());
	}
	return (allowMethods);
}

//Config 내용 프린트 해보는 함수
void	Config::printAllInfo() const {
	if (this->getType() == SERVER_CONFIG) {
		cout << "======== SERVER =========" << endl;
		cout << "type: SERVER_CONFIG" << endl;
		cout << "port: " << port << endl;
		cout << "serverName: " << serverName << endl;
		cout << "root: " << root << endl;
		cout << "clientMaxBodySize: " << clientMaxBodySize << endl;
		cout << "defaultErrorPage: " << defaultErrorPage << endl;
		cout << "index: " << index << endl;
		cout << "autoindex: " << autoindexOn << endl;
		cout << "allowMethods: ";
		for (size_t i = 0; i < this->allowMethods.size(); i++) {
			cout << this->allowMethods[i] << ' ';
		}
		cout << endl;
		for (map<string, Config>::const_iterator it = locations.begin(); it != locations.end(); it++) {
			it->second.printAllInfo();
		}
	} else if (this->getType() == LOCATION_CONFIG) {
		cout << "\t======== LOCATION =========" << endl;
		cout << "\ttype: LOCATION_CONFIG" << endl;
		cout << "\tport: " << port << endl;
		cout << "\tserverName: " << serverName << endl;
		cout << "\troot: " << root << endl;
		cout << "\tclientMaxBodySize: " << clientMaxBodySize << endl;
		cout << "\tdefaultErrorPage: " << defaultErrorPage << endl;
		cout << "\tindex: " << index << endl;
		cout << "\tautoindex: " << autoindexOn << endl;
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