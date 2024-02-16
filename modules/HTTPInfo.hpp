#pragma once
#ifndef HTTPINFO_HPP
#define HTTPINFO_HPP

#include <map>
#include <iostream>

using namespace std;

class HTTPInfo {
	private :
		static map<string, string> properties;
	public :
		static bool isValidStartLine();
		static bool isValidHeaderField();
};

#endif