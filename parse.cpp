#include "parse_config.hpp"

/* config 파일을 읽고, 주석을 제거 후 단어와 특수문자 {, }, ; 로 를 순서대로 분리해 vector에 저장한다.
그리고 지시어는 map<string, vector<string> > 에 저장
블록은 vector<server 저장객체>, vector<loaction 저장객체> 형식으로 저장한다.
*/
ParseConfig::ParseConfig(char *config_file_name) : charset("{};") {
	ifstream	configFile(config_file_name);

	if (configFile.is_open() == false) {
		exitWithErrmsg(string("Open Error: ") + config_file_name);
	}
	string		line;
	while (getline(configFile, line)) {
		lawLines.push_back(line);
	}
	deleteComment();
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
	blockset.push_back("http");
	blockset.push_back("server");
	blockset.push_back("location");
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