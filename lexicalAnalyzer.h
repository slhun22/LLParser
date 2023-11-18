#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#define IDENT 10000
#define CONST 10001
#define ASSIGN_OP 10002
#define SEMI_COLON 10003
#define ADD_OP 10004
#define MUL_OP 10005
#define LEFT_P 10006
#define RIGHT_P 10007
#define END -1
#define ERROR 0

using namespace std;

vector<string> split(string input, char delimiter);

class LexAnalyzer
{
public:
	void lexical();//현재 앞에 있는 문자열과 그 타입을 token_string과 next_token에 저장해주는 함수
	void tokenize();//텍스트 파일에서 읽은 문자열을 lexeme 단위로 끊어서 큐에 저장하는 함수
	void printAllTokens();//-v 옵션 실행 함수
	void loadFileData(string);//텍스트 파일을 읽어 오는 함수
	int getNextToken();//next_token 반환
	string getTokenString();//token_string반환
	string checkNextTokenString();//다음 스트링 미리 보기
	int printLexErrors();//에러 큐에 있는 에러 내용 전부 꺼내서 출력하기
private:
	int next_token = 0;
	string token_string;
	queue<string> q;
	string fileData;
	queue<string> lexicalErrorContainer;
};