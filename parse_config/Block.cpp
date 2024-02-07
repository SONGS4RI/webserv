#include "Block.hpp"
#include <iostream>

Block::Block(string _type) : type(_type) {}

/*지시어에 argument들을 vector<string> 으로 저장해 return*/
vector<string>	Block::getValue(vector<string>::iterator& word)
{
	vector<string>	value;
	for (; *word != ";"; word++) {
		value.push_back(*word);
	}
	return (value);
}

/*블록 내용을 프린트*/
void	Block::printDirectives()
{
	cout << "[" << type << "]" << endl;
	for (map<string, vector<string> >::iterator it = directives.begin(); it != directives.end(); it++) {
		cout << it->first << " : ";
		for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++) {
			cout << *itt << ' ';
		}
		cout << endl;
	}
}
