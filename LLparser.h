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
	unordered_map<string, float> symbolTable;//심볼테이블
	vector<string> symbolOrderContainer;//symbolTable 자체는 순서대로 값을 저장하지 않아 별도로 순서대로 저장하는 sequence container 추가
	vector<string> sentenceContainer; //분석한 문장의 토큰을 가지고 있다가 subResult 출력 때 합쳐서 출력하는 용도
	queue<string> undefinedVariableQueue;//정의되지 않은 변수를 감지할 시 잠깐 보관해두는 큐. subResult 출력에 사용

	int idCount = 0;
	int constCount = 0;
	int opCount = 0;

	string lvalue;

	//사칙연산 중복 오류 복구 관련 변수
	bool isAddOpRedundant;
	bool isSubOpRedundant;
	bool isMulOpRedundant;
	bool isDivOpRedundant;

	//세미콜론 중복 오류 복구 변수
	bool isSemiColonRedundant;

	//세미콜론 오용 오류 복구 변수
	bool isSemiColonWrongPlace;

	//복구 불가한 오류에 대한 변수
	bool isError;

};


