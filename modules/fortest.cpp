#include "fortest.hpp"

string	fileToString(const char* filename) {
	ifstream	file(filename);
	string		str;
	if (file.is_open() == false) {
		exitWithErrmsg("Error: open fail");
	}
	file.seekg(0, file.end);
	int	length = file.tellg();
	file.seekg(0, file.beg);
	str.resize(length);
	file.read(&str[0], length);
	file.close();
	return (str);
}