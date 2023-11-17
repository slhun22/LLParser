#include "lexicalAnalyzer.h"
#include <fstream>
#include <sstream>

vector<string> split(string input, char delimiter)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

void LexAnalyzer::lexical()
{
	if (q.empty())
		return;

	bool isLegal;
	token_string = q.front();
	q.pop();

	if (isalpha(token_string[0]))//identifier
	{
		isLegal = true;
		for (int i = 1; i < token_string.length(); i++)
		{
			if (!(isdigit(token_string[i]) || isalpha(token_string[i]) || token_string[i] == '$'))//C언어 identifier rule을 어기는 경우
			{
				lexicalErrorContainer.push("(Error) \"허용되지 않는 변수명입니다.(" + token_string + ")\"\n");
				isLegal = false;
				next_token = ERROR;
				break;
			}
		}

		if (isLegal)
			next_token = IDENT;
	}
	else if (isdigit(token_string[0]))//decimal number
	{
		isLegal = true;
		for (int i = 1; i < token_string.length(); i++)
		{
			if (!isdigit(token_string[i]))
			{
				lexicalErrorContainer.push("(Error) \"잘못된 상수입니다.(" + token_string + ")\"\n");
				isLegal = false;
				next_token = ERROR;
				break;
			}
		}

		if (isLegal)
			next_token = CONST;
	}
	else//연산자 또는 사용하는 기호
	{
		if (token_string == "+" || token_string == "-") next_token = ADD_OP;
		else if (token_string == "*" || token_string == "/") next_token = MUL_OP;
		else if (token_string == ":=") next_token = ASSIGN_OP;
		else if (token_string == ";") next_token = SEMI_COLON;
		else if (token_string == "(") next_token = LEFT_P;
		else if (token_string == ")") next_token = RIGHT_P;
		else if (token_string == "$") next_token = END;
		else
		{
			lexicalErrorContainer.push("(Error) \"알 수 없는 기호입니다.(" + token_string + ")\"\n");
			next_token = ERROR;
		}
	}

	//cout << "string : " << getTokenString() << "\t token: " << getNextToken() << "\n";
}

void LexAnalyzer::tokenize()
{
	vector<string> temp = split(fileData, ' ');

	for (auto iter : temp)
	{
		if (iter != "")
			q.push(iter);
	}
	q.push("$");
}

void LexAnalyzer::loadFileData(string filename)
{
	ifstream f(filename);
	if (f.is_open())
	{
		f.seekg(0, std::ios::end);// 위치 지정자를 파일 끝으로 옮긴다.
		int size = f.tellg();//끝이므로 포인터 위치를 읽으면 파일 크기
		fileData.resize(size);//파일 내용 받아올 문자열 변수 사이즈 재설정
		f.seekg(0, std::ios::beg);//다시 파일 앞으로 이동
		f.read(&fileData[0], size);//파일 내용 전체 읽어서 s에 저장
		char c = 0;
		while (c < 32)//아스키 코드 32 이하를 모두 스페이스바로 간주하기
		{
			while (fileData.find(c) != string::npos)
				fileData.replace(fileData.find(c), 1, " ");

			c++;
		}
	}

	else
		cout << "파일 열기 실패\n";
}

int LexAnalyzer::getNextToken() { return next_token; }
string LexAnalyzer::getTokenString() { return token_string; }

void LexAnalyzer::printLexErrors() 
{
	while (!lexicalErrorContainer.empty())
	{
		cout << lexicalErrorContainer.front();
		lexicalErrorContainer.pop();
	}
}

string LexAnalyzer::checkNextTokenString()
{
	return q.front();
}

void LexAnalyzer::printAllTokens()
{
	while (q.front() != "$")
	{
		cout << "\"" << q.front() << "\"\n";
		q.pop();
	}
}