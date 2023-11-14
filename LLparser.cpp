#include "LLparser.h"

#define UNDEFVALUE 10101

LexAnalyzer lex;//lexical analyzer unit
unordered_map<string, float> symbolTable;//심볼테이블
vector<string> symbolOrderContainer;//symbolTable 자체는 순서대로 값을 저장하지 않아 별도로 순서대로 저장하는 sequence container 추가
vector<string> sentenceContainer; //분석한 문장의 토큰을 가지고 있다가 subResult 출력 때 합쳐서 출력하는 용도
queue<string> undefinedVariableQueue;//정의되지 않은 변수를 감지할 시 잠깐 보관해두는 큐. subResult 출력에 사용

int idCount = 0;
int constCount = 0;
int opCount = 0;
float rvalue = -1;

//**중요 규칙 : terminal symbol을 읽으면(함수 call) 바로 다음줄에 lexical 함수를 부를 것, nonterminal symbol을 읽으면(함수 call) lexical 함수를 부르지 않음. 

void printSubResult()
{
	string line = "";
	for (string iter : sentenceContainer)
		line.append(iter + " ");
	cout << line << "\n";
	cout << "ID: " << idCount << "; " << "CONST: " << constCount << "; " << "OP: " << opCount << ";\n";
	//cout << "(OK)\n";
	//ok, warning, error 관련 상당히 긴 예외처리문이 필요할 듯 
	while (!undefinedVariableQueue.empty())
	{
		cout << "(Error) \"정의되지 않은 변수(";
		cout << undefinedVariableQueue.front();
		cout << ")가 참조됨\"\n";
		undefinedVariableQueue.pop();
	}

	idCount = 0;
	constCount = 0;
	opCount = 0;
	sentenceContainer.clear();
}

void printFinalResult()
{
	cout << "Result ==> ";
	for (auto iter : symbolOrderContainer)
	{
		if (symbolTable[iter] == UNDEFVALUE)
			cout << iter << ": " << "Unknown; ";
		else
			cout << iter << ": " << symbolTable[iter] << "; ";
	}
	cout << "\n";
}

void left_paren()//<left_paren> → (                                    
{
	sentenceContainer.push_back(lex.getTokenString());
}

void right_paren()//<right_paren> → )                                 
{
	sentenceContainer.push_back(lex.getTokenString());
}

void mult_op()//<mult_operator> → * | /                                
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void add_op()//<add_operator> → + | -      
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void semi_colon()//<semi_colon> → ;                                  
{
	sentenceContainer.push_back(lex.getTokenString());
	printSubResult();
}

void assignment_op()//<assignment_op> → :=                       
{
	sentenceContainer.push_back(lex.getTokenString());
}

void ident()//<ident> → any names conforming to C identifier rules    
{
	sentenceContainer.push_back(lex.getTokenString());
	idCount++;
}

void constant()//<const> → any decimal numbers                       
{
	sentenceContainer.push_back(lex.getTokenString());
	constCount++;
}

float factor()//<factor> → <left_paren><expression><right_paren> | <ident> | <const>
{
	float v1;
	switch (lex.getNextToken())
	{
	case LEFT_P:
		left_paren();
		lex.lexical();
		v1 = expression();
		if (lex.getNextToken() == RIGHT_P)
		{
			right_paren();
			return v1;
		}
		else
		{
			//error
		}
		break;
	case IDENT:
		ident();
		if (symbolTable.find(lex.getTokenString()) != symbolTable.end())//변수가 table에 존재하는 경우
		{
			return symbolTable[lex.getTokenString()];
		}
		else//변수가 table에 존재하지 않는 경우
		{
			undefinedVariableQueue.push(lex.getTokenString());
			symbolTable[lex.getTokenString()] = UNDEFVALUE;
			symbolOrderContainer.push_back(lex.getTokenString());
			return UNDEFVALUE;
		}
		break;
	case CONST:
		constant();
		return stof(lex.getTokenString());
		break;
	default: //error?
		break;
	}
}

float factor_tail(float beforeFactor)//<factor_tail> → <mult_op><factor><factor_tail> | ε
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case MUL_OP :
		mult_op();
		op = lex.getTokenString();
		lex.lexical();
		v1 = factor();
		lex.lexical();

		if (v1 == UNDEFVALUE || beforeFactor == UNDEFVALUE)
			result = UNDEFVALUE;
		else
		{
			if (op == "*")
				result = beforeFactor * v1;
			else
				result = beforeFactor / v1;
		}
		return factor_tail(result);
		break;
	default :
		return beforeFactor;
		break;
	}
}

float term()//<term> → <factor> <factor_tail>
{
	float temp;
	float v1 = factor();
	lex.lexical();
	float v2 = factor_tail(v1);
	return v2;
}

float term_tail(float beforeTerm)//<term_tail> → <add_op><term><term_tail> | ε
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case ADD_OP :
		add_op();
		op = lex.getTokenString();
		lex.lexical();
		v1 = term();
		if (v1 == UNDEFVALUE || beforeTerm == UNDEFVALUE)
			result = UNDEFVALUE;
		else
		{
			if (op == "+")
				result = beforeTerm + v1;
			else
				result = beforeTerm - v1;
		}	
		return term_tail(result);
		break;
	default :
		return beforeTerm;
		break;
	}
}

float expression()//<expression> → <term><term_tail>
{
	float v1 = term();
	float v2 = term_tail(v1);
	return v2;
}

void statement()//<statement> → <ident><assignment_op><expression>
{
	string lvalue;
	float rvalue;
	switch (lex.getNextToken())
	{
	case IDENT :
		ident();
		lvalue = lex.getTokenString();
		lex.lexical();
		if (lex.getNextToken() == ASSIGN_OP)
		{
			assignment_op();
			lex.lexical();
			rvalue = expression();
			symbolTable[lvalue] = rvalue;
			bool exist = false;
			for (string iter : symbolOrderContainer)
				if (iter == lvalue)
					exist = true;
			if(!exist)//같은 변수에 대한 연산이 등장할 것을 고려하여 이미 앞에서 정의한 변수가 아닌 새로운 변수일 경우에만 순서에 추가
				symbolOrderContainer.push_back(lvalue);
		}
		else
		{
			//error
			break;
		}
		break;

	default : 
		//error
		break;
	}
	
}

void statements()//<statements> → <statement> | <statement><semi_colon><statements>
{
	statement();
	switch (lex.getNextToken())
	{
	case SEMI_COLON : 
		semi_colon();
		lex.lexical();
		statements();
		break;
	case END:
		break;
	default :
		//error
		break;
	}
}

void program()//<program> → <statements>
{
	lex.lexical();//initial lexical before start parsing
	statements();
	//트리 순회 끝
	printSubResult();
	printFinalResult();
}

int main()
{
	lex.loadFileData();
	lex.tokenize();
	program();
}