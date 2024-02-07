#ifndef BLOCK_HPP
# define BLOCK_HPP

# include <string>
# include <map>
# include <vector>

using namespace std;

class Block {
	public:
		string							type;
		string							location;
		map<string, vector<string> >	directives;
		vector<Block>					blocks;
		static vector<string>	getValue(vector<string>::iterator& word);
		void	printDirectives();
		Block(string _type);
};

#endif
