#include <sstream>
#include "Utils.hpp"

using namespace std;

void	Utils::exitWithErrmsg(const string& errmsg) {
    log(errmsg, RED);
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

void Utils::log(const string& msg, const string& color) {
    cout << color << msg << RESET << endl;
}