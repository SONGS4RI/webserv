#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <string>

#define RESET "\033[0m"
#define GRAY "\033[1;90m"
#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define YELLOW "\033[1;93m"
#define BLUE "\033[1;94m"
#define MAGENTA "\033[1;95m"
#define CYAN "\033[1;96m"

using namespace std;

class Utils {
	private:
	public:
		~Utils();
		static void exitWithErrmsg(const string& errmsg);
		static string intToString(const int& number);
		static size_t hexToDecimal(const string& hex);
		static void log(const string& msg, const string& color);
};

#endif