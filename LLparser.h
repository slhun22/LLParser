#pragma once

#include "lexicalAnalyzer.h"
#include <unordered_map>

class LLparser
{
public:
	void printSubResult();
	void printFinalResult();
	void error();
	void left_paren();
	void right_paren();
	void mult_op();
	void add_op();
	void semi_colon();
	void assignment_op();
	void ident();
	void constant();
	float factor();
	float factor_tail(float);
	float term();
	float term_tail(float);
	float expression();
	void statement();
	void statements();
	void program();

	LexAnalyzer lex;//lexical analyzer unit
private:
	unordered_map<string, float> symbolTable;//�ɺ����̺�
	vector<string> symbolOrderContainer;//symbolTable ��ü�� ������� ���� �������� �ʾ� ������ ������� �����ϴ� sequence container �߰�
	vector<string> sentenceContainer; //�м��� ������ ��ū�� ������ �ִٰ� subResult ��� �� ���ļ� ����ϴ� �뵵
	queue<string> undefinedVariableQueue;//���ǵ��� ���� ������ ������ �� ��� �����صδ� ť. subResult ��¿� ���

	int idCount = 0;
	int constCount = 0;
	int opCount = 0;

	string lvalue;

	//��Ģ���� �ߺ� ���� ���� ���� ����
	bool isAddOpRedundant;
	bool isSubOpRedundant;
	bool isMulOpRedundant;
	bool isDivOpRedundant;

	//�����ݷ� �ߺ� ���� ���� ����
	bool isSemiColonRedundant;

	//�����ݷ� ���� ���� ���� ����
	bool isSemiColonWrongPlace;

	//���� �Ұ��� ������ ���� ����
	bool isError;

};


