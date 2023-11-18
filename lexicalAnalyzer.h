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
	void lexical();//���� �տ� �ִ� ���ڿ��� �� Ÿ���� token_string�� next_token�� �������ִ� �Լ�
	void tokenize();//�ؽ�Ʈ ���Ͽ��� ���� ���ڿ��� lexeme ������ ��� ť�� �����ϴ� �Լ�
	void printAllTokens();//-v �ɼ� ���� �Լ�
	void loadFileData(string);//�ؽ�Ʈ ������ �о� ���� �Լ�
	int getNextToken();//next_token ��ȯ
	string getTokenString();//token_string��ȯ
	string checkNextTokenString();//���� ��Ʈ�� �̸� ����
	int printLexErrors();//���� ť�� �ִ� ���� ���� ���� ������ ����ϱ�
private:
	int next_token = 0;
	string token_string;
	queue<string> q;
	string fileData;
	queue<string> lexicalErrorContainer;
};