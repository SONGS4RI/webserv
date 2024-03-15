#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "../utils/Enums.hpp"
#include "ParseConfig.hpp"

#define BUF_SIZE 1024

using namespace std;

class Config {
	private:
		EConfigType	type;
		map<string, Config>	locations;
		/*서버에만 있음*/
		int		port;//default 8080 unsigned short range 벗어나면 에러
		string	serverName;//default webserv.com
		string	root;//default /
		string	defaultErrorPage;//default 빈 문자열
		bool	autoindexOn;//default on
		/*서버, 로케이션 둘 다 있을 수 있음*/
		string	index;//default 빈 문자열
		size_t	clientMaxBodySize;//default 1000000, int range벗어나면 에러
		vector<string>	allowMethods;//default 비어있음
		/* locations 에만 적용, 서버에는 비어있음 */
		string	alias;//default 빈 문자열

		bool	isWrongClientMaxBodySize(const string& CMBS_String) const;
		bool	isWrongPort(const string& portString) const;
		bool	isWrongAllowMethod(const string& allowMethods_) const;
		bool	isWrongPath(const string& path);
	public:
		Config();
		Config(Block& globalBlock, Block& serverBlock);
		Config(Config& serverConfig, Block* locationBlock);
		void	setByBlock(Block& Block_);
		
		/* Getter */
		const EConfigType	getType() const;
		const int&	getPort() const;
		const string&	getServerName() const;
		const string&	getRoot() const;
		/* location 에 있을 수 있는 지시어들.
		loc없는건 서버 config에서 가져옴
		loc있는건 location config에 있으면 가져오고 없으면 서버config에서 가져옴*/
		const size_t& getClientMaxBodySize() const;
		const size_t& getClientMaxBodySize(string loc) const;
		const string&	getDefaultErrorPage() const;
		const string	getIndex() const;
		const string	getIndex(string loc) const;
		const bool&	getAutoindexOn() const;
		const bool&	getAutoindexOn(string loc) const;
		const vector<string>&	getAllowMethods() const;
		const vector<string>&	getAllowMethods(string loc) const;

		void	printAllInfo() const;
};

#endif