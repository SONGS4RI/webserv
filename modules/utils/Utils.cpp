#include <sstream>
#include "Utils.hpp"

using namespace std;

void	Utils::exitWithErrmsg(const string& errmsg) {
	std::cout << errmsg << std::endl;
	exit(1);
}

string Utils::intToString(const int& number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

size_t Utils::hexToDecimal(const string& hex) {
    return strtol(hex.c_str(), NULL, 16);
};