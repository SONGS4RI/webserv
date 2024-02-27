#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <vector>
#include <string>
#include "./enum/Enums.hpp"
#include "ParseConfig.hpp"

#define BUF_SIZE 1024

using namespace std;

class Config {
	private:
		EConfigType	type;
		map<string, Config>	locations;

		int		port;//default 8080 unsigned short range 벗어나면 에러
		string	serverName;//default webserv.com
		size_t	clientMaxBodySize;//default 1000000, int range벗어나면 에러
		string	root;//default /
		string	index;//default 빈 문자열 에러처리 구현???
		bool	autoindexOn;//default off
		/* locations 에만 적용, 서버에는 비어있음 */
		string			returnRedir;//default location
		vector<string>	allowMethods;//default 비어있음
		string			alias;//default 비어있음 꼭 써야할까???

		bool	isWrongClientMaxBodySize(const string& CMBS_String) const;
		bool	isWrongPort(const string& portString) const;
		bool	isWrongAllowMethod(const string& allowMethods_) const;
		bool	isWrongPath(const string& path);
	public:
		Config();
		Config(Block& globalBlock, Block& serverBlock);
		Config(Config& serverConfig, Block& locationBlock);
		void	setByBlock(Block& Block_);
		
		/* Getter */
		EConfigType	getType();
		map<string, Config>	getLocations();
		int	getPort();
		string	getServerName();
		size_t	getClientMaxBodySize();
		string	getRoot();
		string	getIndex();
		bool	getAutoindexOn();
		string	getReturnRedir();
		vector<string>	getAllowMethods();
		// static vector<Config> parse(char* fileName);// 파일 파싱하면서 서버 블록으로 나눈 벡터
		void	printAllInfo();
};

#endif