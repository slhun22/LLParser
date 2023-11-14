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
			if (!(isdigit(token_string[i]) || isalpha(token_string[i]) || token_string[i] == '$'))//C��� identifier rule�� ���� ���
			{
				cout << "(Error) \"������ �ʴ� �������Դϴ�.\"\n";
				isLegal = false;
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
				cout << "(Error) \"�߸��� ����Դϴ�.\"\n";
				isLegal = false;
				break;
			}
		}

		if (isLegal)
			next_token = CONST;
	}
	else//������
	{
		if (token_string == "+" || token_string == "-") next_token = ADD_OP;
		else if (token_string == "*" || token_string == "/") next_token = MUL_OP;
		else if (token_string == ":=") next_token = ASSIGN_OP;
		else if (token_string == ";") next_token = SEMI_COLON;
		else if (token_string == "(") next_token = LEFT_P;
		else if (token_string == ")") next_token = RIGHT_P;
		else if (token_string == "$") next_token = END;
		else
			cout << "(Error) \"�� �� ���� ��ȣ�Դϴ�.\"\n";
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

void LexAnalyzer::loadFileData()
{
	ifstream f("eval1.txt");
	if (f.is_open())
	{
		f.seekg(0, std::ios::end);// ��ġ �����ڸ� ���� ������ �ű��.
		int size = f.tellg();//���̹Ƿ� ������ ��ġ�� ������ ���� ũ��
		fileData.resize(size);//���� ���� �޾ƿ� ���ڿ� ���� ������ �缳��
		f.seekg(0, std::ios::beg);//�ٽ� ���� ������ �̵�
		f.read(&fileData[0], size);//���� ���� ��ü �о s�� ����
		char c = 0;
		while (c < 32)//�ƽ�Ű �ڵ� 32 ���ϸ� ��� �����̽��ٷ� �����ϱ�
		{
			while (fileData.find(c) != string::npos)
				fileData.replace(fileData.find(c), 1, " ");

			c++;
		}
	}

	else
		cout << "���� ���� ����\n";
}

int LexAnalyzer::getNextToken() { return next_token; }
string LexAnalyzer::getTokenString() { return token_string; }