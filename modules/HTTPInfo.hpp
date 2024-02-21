#pragma once
#ifndef HTTPINFO_HPP
#define HTTPINFO_HPP

#include <map>
#include <iostream>

using namespace std;

class HTTPInfo {
	private :
		static map<string, vector<string> > allowedHeaderField;
	public :
		static void init();
		static bool isValidStartLine(const string& method, const string& requestUrl, const string& httpVersion);
		static bool isValidHeaderField(const map<string, string>& properties);
};

#endif