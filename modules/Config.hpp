#pragma once

#include <vector>

#define BUF_SIZE 1024

using namespace std;

class Config {
	private:
		Config(/* 서버별 블록 객체가 있는게 편하면 이때 집어 넣어서 인스턴스 생성하면 될듯 */);//
	public:
		~Config();// conf 파일 닫기
		static vector<Config> parse(char* fileName);// 파일 파싱하면서 서버 블록으로 나눈 벡터
		/*
		
			파싱한 서버 블록별 데이터
		
		*/
};