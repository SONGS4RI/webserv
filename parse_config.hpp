#ifndef PARSE_CONFIG_HPP
# define PARSE_CONFIG_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <map>

# include "utility.hpp"

using namespace std;

class ParseConfig {
	private:
		string			charset;
		vector<string>	blockset;
		vector<string>	lawLines;
		vector<string>	words;
		map<string, vector<string> >	directives;
		HttpConfig						httpConfigs;
		string	nextWord(string line, size_t& idx);
		bool	isCharsetInString(const string& str, const string charset);
		bool	isCharInCharset(char c, const string charset);
		void	pushMixedWords(const string& word, vector<string>& words);
		void	deleteComment();
	public:
		ParseConfig(char *config_file_name);

};
class HttpConfig {
	private:
	map<string, vector<string> >	directives;
	vector<ServerConfig>			serverConfigs;
	public:	
};
class ServerConfig {
	private:
	map<string, vector<string> >	directives;
	vector<LoactionConfig>			locationsConfigs;
	public:	
};
class LoactionConfig {
	private:
	map<string, vector<string> >	directives;
	public:	
};

#endif