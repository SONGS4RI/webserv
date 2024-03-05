#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

class Utils {
	private:
	public:
		~Utils();
		static void exitWithErrmsg(const string& errmsg);
		static string intToString(const int& number);
		static size_t hexToDecimal(const string& hex);
};

#endif