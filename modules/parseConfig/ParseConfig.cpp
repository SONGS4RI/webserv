#include "ParseConfig.hpp"

/* config 파일을 읽고, 주석을 제거 후 단어와 특수문자 {, }, ; 로 를 순서대로 분리해 vector에 저장한다.
그리고 지시어는 map<string, vector<string> > 에 저장
블록은 vector<server 저장객체>, vector<loaction 저장객체> 형식으로 저장한다.
*/
ParseConfig::ParseConfig(const char* configFileName) : charset("{};"), globalBlock(Block("global")) {
	setLawLines(configFileName);
	deleteComment();
	setWords();
	checkWrongSyntaxInConfig();
	try	{
		fillBlocks(words.begin(), 0, globalBlock);
	} catch(const char * e) {
		Utils::exitWithErrmsg(e);
	}
	checkBlocksWrong();
	setServerConfigs();
	//printAllBlocks();
}

/*config 파일 읽고 lawlines에 저장*/
void	ParseConfig::setLawLines(const char* configFileName)
{
	ifstream	configFile(configFileName);

	if (configFile.is_open() == false) {
		Utils::exitWithErrmsg(string("Open Error: ") + configFileName);
	}
	string		line;
	while (getline(configFile, line)) {
		lawLines.push_back(line);
	}
}

/*words 에 토큰 저장*/
void	ParseConfig::setWords()
{
	string	new_word;
	for (size_t i = 0; i < lawLines.size(); i++) {
		for (size_t j = 0; j < lawLines[i].size(); j++) {
			new_word = nextWord(lawLines[i], j);
			if (new_word.size() == 0) {
				continue ;
			} else if (isCharsetInString(new_word, charset)) {
				pushMixedWords(new_word, words);
			} else {
				words.push_back(new_word);
			}
		}
	}
}

bool	ParseConfig::checkWrongSyntaxInConfig() {
	if (words.size() == 0) {
		Utils::exitWithErrmsg("Error: empty config file");
	} else if (isBraceError() == true) {
		Utils::exitWithErrmsg("Error: Brace {} is wrong");
	} else if (isFrontOpenBraceError() == true) {
		Utils::exitWithErrmsg("Error: front of { is wrong");
	} else if (isFrontCloseBraceError() == true) {
		Utils::exitWithErrmsg("Error: front of } is wrong");
	}
	try {
		isWordSyntaxError();
	} catch (const char * errmsg) {
		Utils::exitWithErrmsg(errmsg);
	}
	return (false);
}

bool	ParseConfig::isWordSyntaxError() {
	vector<string>::iterator back;
	vector<string>::iterator backback;
	vector<string> strset;
	strset.push_back("http");
	strset.push_back("server");
	strset.push_back("location");

	for (vector<string>::iterator it = words.begin(); it != words.end(); ++it) {
		if (it->size() == 1  && isCharInCharset((*it)[0], charset) == true) {
			continue ;
		} else if (isStrInStrset(*it, strset) == false) {
			back = it + 1;
			if (back == words.end()) {
				throw "Error: configfile syntax error";
			}
			if (*back == ";") {
				throw "Error: right after directive can't be ';'";
			}
			++it;
			while (1) {
				if (it == words.end()) {
					throw "Error: configfile syntax error";
				} else if (*it == ";") {
					break ;
				}
				++it;
			}
		}
		else if (*it == "http" || *it == "server") {
			back = it + 1;
			if (back == words.end()) {
				throw "Error: right after 'http/server' keyword must be '{'";
			} else if (*back != "{") {
				throw "Error: right after 'http/server' keyword must be '{'";
			}
		} else if (*it == "location") {
			back = it + 1;
			if (back == words.end()) {
				throw "Error: right after 'location' keyword must be location";
			}
			backback = back + 1;
			if (backback == words.end()) {
				throw "Error: after location must be '{'";
			} else if (back->size() == 1 && isCharInCharset((*back)[0], charset) == true) {
				throw "Error: right after 'location' keyword must be location";
			} else if (*backback != "{") {
				throw "Error: after location must be '{'";
			}
		}
	}
	return (false);
}

bool	ParseConfig::isFrontCloseBraceError() {
	vector<string>::reverse_iterator front;

	for (vector<string>::reverse_iterator rit = words.rbegin(); rit != words.rend(); rit++) {
		if (*rit == "}") {
			front = rit + 1;
			if (front == words.rend()) {
				return (true);
			} else if (front->size() != 1) {
				return (true);
			} else if (isCharInCharset((*front)[0], charset) == false) {
				return (true);
			}
		}
	}
	return (false);
}

bool	ParseConfig::isFrontOpenBraceError()
{
	vector<string>::reverse_iterator front;

	for (vector<string>::reverse_iterator rit = words.rbegin(); rit != words.rend(); rit++) {
		if (*rit == "{") {
			front = rit + 1;
			if (front == words.rend()) {
				return (true);
			} else if (front->size() == 1) {
				if (isCharInCharset((*front)[0], charset) == true) {
					return (true);
				}
			}
		}
	}
	return (false);
}

/*중괄호 짝이 안맞거나 열기 전에 닫으면 error*/
bool	ParseConfig::isBraceError()
{
	int	i = 0;
	for (vector<string>::iterator it = words.begin(); it != words.end(); ++it) {
		if (*it == "{") {
			++i;
		} else if (*it == "}") {
			--i;
			if (i < 0) {
				return (true);
			}
		}
	}
	if (i != 0) {
		return (true);
	}
	return (false);
}

/*모든 블록을 프린트해보는 함수*/
void	ParseConfig::printAllBlocks()
{
	globalBlock.printDirectives();
	for (size_t i = 0; i < globalBlock.blocks.size(); i++)
	{
		globalBlock.blocks[i].printDirectives();
		for (size_t j = 0; j < globalBlock.blocks[i].blocks.size(); j++)
		{
			globalBlock.blocks[i].blocks[j].printDirectives();
			for (size_t k = 0; k < globalBlock.blocks[i].blocks[j].blocks.size(); k++)
			{
				globalBlock.blocks[i].blocks[j].blocks[k].printDirectives();
			}
		}
	}
}

/*words 를 읽고 globalBlock에 전부 집어넣는 함수*/
vector<string>::iterator	ParseConfig::fillBlocks(vector<string>::iterator it, size_t depth, Block& localBlock) {
	for (; it != words.end(); it++) {
		if (*it == "http") {
			if (depth != 0) {
				throw "Error: in config file 'http' block's position is wrong";
			}
			Block	newBlock("http");
			localBlock.blocks.push_back(newBlock);
			it = fillBlocks(it + 2, depth + 1, *(localBlock.blocks.end() - 1));
		} else if (*it == "server") {
			if (depth != 1) {
				throw "Error: in config file 'server' block's position is wrong";
			}
			Block	newBlock("server");
			localBlock.blocks.push_back(newBlock);
			it = fillBlocks(it + 2, depth + 1, *(localBlock.blocks.end() - 1));
		} else if (*it == "location") {
			if (depth != 2) {
				throw "Error: in config file 'location' block's position is wrong";
			}
			Block	newBlock("location");
			it = setLocation(it, newBlock);
			localBlock.blocks.push_back(newBlock);
			it = fillBlocks(it + 1, depth + 1, *(localBlock.blocks.end() - 1));
		} else if (*it == "}") {
			return (it);
		} else if (isSkipBlock(it) == true) {
			it = skipThisBlock(it);
		} else if (*it == ";") {
			continue ;
		}
		else {
			string	key = *it;
			it++;
			localBlock.directives[key] = Block::getValue(it);
		}
	}
	return (it);
}

/*fillBlocks 함수에서 블록을 스킵할지 정하는 함수*/
bool	ParseConfig::isSkipBlock(vector<string>::iterator it)
{
	while (1)
	{
		if (*it == "{") {
			return (true);
		} else if (*it == ";") {
			return (false);
		}
		it++;
	}
}

/*블록을 스킵하는 함수*/
vector<string>::iterator	ParseConfig::skipThisBlock(vector<string>::iterator word) {
	int	depth = 0;

	while (*word != "}" || depth != 1) {
		if (*word == "{") {
			depth++;
		} else if (*word == "}") {
			depth--;
		}
		++word;
	}
	return (word);
}

/*Location 의 주소를 저장하고 { 위치의 iterator를 return*/
vector<string>::iterator	ParseConfig::setLocation(vector<string>::iterator it, Block& block) {
	while (*it != "{") {
		++it;
	}
	block.location = *(it - 1);
	return (it);
}

/* charset이 섞인 문자덩어리를 charset과 문자열로 분리해서 words 벡터에 pushback 하는 함수 */
void	ParseConfig::pushMixedWords(const string& word, vector<string>& words) {
	for (size_t i = 0; i < word.size(); i++) {
		string	new_word;
		words.push_back(new_word);
		if (isCharInCharset(word[i], charset) == true) {
			(words.end() - 1)->push_back(word[i]);
		} else {
			for (; isCharInCharset(word[i], charset) == false && word[i] != 0; i++) {
				(words.end() - 1)->push_back(word[i]);
			}
			if (isCharInCharset(word[i], charset) == true) {
				i--;
			}
		}
	}
}
/* string에서 다음 문자 덩어리를 찾아서 return 하는 함수 */
string	ParseConfig::nextWord(string line, size_t& idx) {
	string	new_word;

	while (isspace(line[idx]) == true) {
		idx++;
	}
	for (; (idx < line.size()) && isspace(line[idx]) == false; ++idx) {
		new_word.push_back(line[idx]);
	}
	return (new_word);
}
/* string 에 charset 의 문자가 포함되는지 확인하는 함수 */
bool	ParseConfig::isCharsetInString(const string& str, const string charset) {
	for (size_t i = 0; i < str.size(); i++) {
		if (isCharInCharset(str[i], charset) == true) {
			return (true);
		}
	}
	return (false);
}
/* char c가 charset 에 포함되는지 확인하는 함수 */
bool	ParseConfig::isCharInCharset(char c, const string charset) {
	for (size_t i = 0; i < charset.size(); i++) {
		if (c == charset[i]) {
			return (true);
		}
	}
	return (false);
}

bool	ParseConfig::isStrInStrset(const string& str, const vector<string>& strset) {
	for (size_t i = 0; i < strset.size(); i++) {
		if (str == strset[i]) {
			return (true);
		}
	}
	return (false);
}

/* config 파일에서 주석을 삭제하는 함수 */
void	ParseConfig::deleteComment() {
	size_t	pos;
	for (size_t i = 0; i < lawLines.size(); i++) {
		pos = lawLines[i].find("#", 0);
		if (pos != SIZE_T_MAX) {
			lawLines[i].erase(pos);
		}
	}
}

void	ParseConfig::checkBlocksWrong() {
	if (globalBlock.blocks.size() != 1) {
		Utils::exitWithErrmsg("Error: http block is not 1 in config file");
	} else if (globalBlock.blocks[0].blocks.size() < 1) {
		Utils::exitWithErrmsg("Error: no server block in config file");
	}
}

void	ParseConfig::setServerConfigs() {
	for (size_t i = 0; i < globalBlock.blocks[0].blocks.size(); i++) {
		serverConfigs.push_back(Config(globalBlock, globalBlock.blocks[0].blocks[i]));
	}
}

vector<Config>&	ParseConfig::getServerConfigs() {
	return (serverConfigs);
}
