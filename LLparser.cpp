#include "LLparser.h"

#define UNDEFVALUE 10101

void LLparser::printSubResult()
{
	bool isOK = true;
	string line = "";
	for (string iter : sentenceContainer)
		line.append(iter + " ");
	cout << line << "\n";
	cout << "ID: " << idCount << "; " << "CONST: " << constCount << "; " << "OP: " << opCount << ";\n";

	if (isAddOpRedundant) cout << "(Warning) \"중복 연산자(+) 제거\"\n";
	if (isSubOpRedundant) cout << "(Warning) \"중복 연산자(-) 제거\"\n";
	if (isMulOpRedundant) cout << "(Warning) \"중복 연산자(*) 제거\"\n";
	if (isDivOpRedundant) cout << "(Warning) \"중복 연산자(/) 제거\"\n";
	
	if (isSemiColonRedundant) cout << "(Warning) \"중복 세미콜론 제거\"\n";

	if (isSemiColonWrongPlace) cout << "(Warning) \"마지막 문장의 불필요한 세미콜론 제거\"\n";

	lex.printLexErrors();

	if (isError)
	{
		cout << "(Error) \"문법에 맞지 않는 문장입니다. (복구 불가능)\"\n";
		symbolTable[lvalue] = UNDEFVALUE;
	}		

	if (isAddOpRedundant || isSubOpRedundant || isMulOpRedundant || isDivOpRedundant || isSemiColonRedundant || isSemiColonWrongPlace || isError) isOK = false;

	while (!undefinedVariableQueue.empty())
	{
		cout << "(Error) \"정의되지 않은 변수(";
		cout << undefinedVariableQueue.front();
		cout << ")가 참조됨\"\n";
		undefinedVariableQueue.pop();
		isOK = false;
	}

	if (isOK) 
		cout << "(OK)\n";

	idCount = 0;
	constCount = 0;
	opCount = 0;
	isAddOpRedundant = false;
	isSubOpRedundant = false;
	isMulOpRedundant = false;
	isDivOpRedundant = false;
	isSemiColonRedundant = false;
	isSemiColonWrongPlace = false;
	isError = false;
	sentenceContainer.clear();
}

void LLparser::printFinalResult()
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

void LLparser::error()//복구 불가 에러 대응 함수
{
	isError = true;
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::left_paren()//<left_paren> → (                                    
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::right_paren()//<right_paren> → )                                 
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::mult_op()//<mult_operator> → * | /                                
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void LLparser::add_op()//<add_operator> → + | -      
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void LLparser::semi_colon()//<semi_colon> → ;                                  
{
	sentenceContainer.push_back(lex.getTokenString());
	lex.lexical();
	if (lex.getTokenString() == ";")//세미콜론 중복 확인
	{
		isSemiColonRedundant = true;
		lex.lexical();
	}
	printSubResult();
}

void LLparser::assignment_op()//<assignment_op> → :=                       
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::ident()//<ident> → any names conforming to C identifier rules    
{
	sentenceContainer.push_back(lex.getTokenString());
	idCount++;
}

void LLparser::constant()//<const> → any decimal numbers                       
{
	sentenceContainer.push_back(lex.getTokenString());
	constCount++;
}

float LLparser::factor()//<factor> → <left_paren><expression><right_paren> | <ident> | <const>
{
	float v1;
	switch (lex.getNextToken())
	{
	case LEFT_P:
		left_paren();
		lex.lexical();
		//왼 괄호 중복 시 제거 후 boolean으로 따로 값 넘겨서 마지막 subresult 떄 불값에 따라 warning log 출력 //OK 나 error 관련해서도 radiobutton처럼 bool값 필요
		v1 = expression();
		if (lex.getNextToken() == RIGHT_P)
		{
			right_paren();
			return v1;
		}
		else
		{
			error();
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
	default:
		error();
		//return UNDEFVALUE;
		break;
	}
}

float LLparser::factor_tail(float beforeFactor)//<factor_tail> → <mult_op><factor><factor_tail> | ε
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case MUL_OP :
		mult_op();
		op = lex.getTokenString();
		lex.lexical();
		if (op == lex.getTokenString())//곱셈나눗셈 중복 확인
		{
			if (op == "*")
				isMulOpRedundant = true;
			else
				isDivOpRedundant = true;
			lex.lexical();
		}
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

float LLparser::term()//<term> → <factor> <factor_tail>
{
	float temp;
	float v1 = factor();
	lex.lexical();
	float v2 = factor_tail(v1);
	return v2;
}

float LLparser::term_tail(float beforeTerm)//<term_tail> → <add_op><term><term_tail> | ε
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case ADD_OP :
		add_op();
		op = lex.getTokenString();
		lex.lexical();
		if (op == lex.getTokenString())//덧셈뺄셈 중복 확인
		{
			if (op == "+")
				isAddOpRedundant = true;
			else
				isSubOpRedundant = true;
			lex.lexical();
		}
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

float LLparser::expression()//<expression> → <term><term_tail>
{
	float v1 = term();
	float v2 = term_tail(v1);
	return v2;
}

void LLparser::statement()//<statement> → <ident><assignment_op><expression>
{
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
			error();
			break;
		}
		break;

	default : 
		error();
		break;
	}
	
}

void LLparser::statements()//<statements> → <statement> | <statement><semi_colon><statements>
{
	statement();

	switch (lex.getNextToken())
	{
	case SEMI_COLON : 
		if (lex.checkNextTokenString() == "$") //마지막 문장인데 세미콜론이 붙어있는 경우
		{
			isSemiColonWrongPlace = true;
			return;
		}
		semi_colon();
		//lex.lexical();   //세미콜론만 예외처리 구조상 semi_colon 함수 내부에 lexical함수를 넣었음.
		statements();
		break;
	case END:
		break;
	default :
		error();
		break;
	}
}

void LLparser::program()//<program> → <statements>
{
	lex.lexical();//initial lexical before start parsing
	statements();
	//트리 순회 끝
	printSubResult();
	printFinalResult();
}