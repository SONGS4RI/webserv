#include <sstream>
#include "Utils.hpp"

using namespace std;

void	exitWithErrmsg(std::string errmsg) {
	std::cout << errmsg << std::endl;
	exit(1);
}

std::string intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}