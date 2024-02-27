#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <map>

# include "Utils.hpp"
# include "Block.hpp"
# include "Config.hpp"

using namespace std;

class Config;

class ParseConfig {
	private:
		string			charset;
		vector<string>	lawLines;
		vector<string>	words;
		Block			globalBlock;

		vector<Config>	serverConfigs;

		void	setLawLines(const char* configFileName);
		void	deleteComment();
		void	setWords();
		string	nextWord(string line, size_t& idx);
		void	pushMixedWords(const string& word, vector<string>& words);

		bool	checkWrongSyntaxInConfig();
		bool	isBraceError();
		bool	isFrontOpenBraceError();
		bool	isFrontCloseBraceError();
		bool	isWordSyntaxError();
		bool	isStrInStrset(const string& str, const vector<string>& strset);
		vector<string>::iterator	fillBlocks(vector<string>::iterator it, size_t depth, Block& localBlock);
		vector<string>::iterator	skipThisBlock(vector<string>::iterator it);
		void	printAllBlocks();
		vector<string>::iterator	setLocation(vector<string>::iterator it, Block& block);

		bool	isCharsetInString(const string& str, const string charset);
		bool	isCharInCharset(char c, const string charset);
		bool	isSkipBlock(vector<string>::iterator it);

		void	checkBlocksWrong();

		void	setServerConfigs();
		public:
		ParseConfig(const char* configFileName);
		vector<Config>&	getServerConfigs();
};

#endif
