#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <vector>
#include <string>
#include "./enum/Enums.hpp"
#include "./parseConfig/parseConfig.hpp"

#define BUF_SIZE 1024

using namespace std;

class Config {
	private:
		EConfigType	type;
		map<string, Config>	locations;
		int		port;
		string	serverName;
		size_t	clientMaxBodySize;
		string	root;
		string	index;
		bool	autoindexOn;
		/* location 블록에만 적용 */
		string			returnRedir;
		vector<string>	allowMethods;
	public:
		Config();
		Config(Block& globalBlock, Block& serverBlock);
		Config(Config& serverConfig, Block& locationBlock);
		void	setByGlobalBlock(const Block& globalBlock);
		void	setByServerBlock(const Block& serverBlock);
		void	setByLocationBlock(Block& locationBlock);
		//~Config();// conf 파일 닫기
		
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
		void	printAllInfo(Config& con);
};

#endif