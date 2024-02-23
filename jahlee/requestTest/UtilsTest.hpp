#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdlib>
#include <string>

using namespace std;

class UtilsTest {
	private:
	public:
		~UtilsTest();

		static size_t hexToDecimal(const string& hex) {
			return strtol(hex.c_str(), NULL, 16);
		};
};

#endif