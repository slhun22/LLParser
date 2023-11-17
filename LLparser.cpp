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

	if (isAddOpRedundant) cout << "(Warning) \"�ߺ� ������(+) ����\"\n";
	if (isSubOpRedundant) cout << "(Warning) \"�ߺ� ������(-) ����\"\n";
	if (isMulOpRedundant) cout << "(Warning) \"�ߺ� ������(*) ����\"\n";
	if (isDivOpRedundant) cout << "(Warning) \"�ߺ� ������(/) ����\"\n";
	
	if (isSemiColonRedundant) cout << "(Warning) \"�ߺ� �����ݷ� ����\"\n";

	if (isSemiColonWrongPlace) cout << "(Warning) \"������ ������ ���ʿ��� �����ݷ� ����\"\n";

	lex.printLexErrors();

	if (isError)
	{
		cout << "(Error) \"������ ���� �ʴ� �����Դϴ�. (���� �Ұ���)\"\n";
		symbolTable[lvalue] = UNDEFVALUE;
	}		

	if (isAddOpRedundant || isSubOpRedundant || isMulOpRedundant || isDivOpRedundant || isSemiColonRedundant || isSemiColonWrongPlace || isError) isOK = false;

	while (!undefinedVariableQueue.empty())
	{
		cout << "(Error) \"���ǵ��� ���� ����(";
		cout << undefinedVariableQueue.front();
		cout << ")�� ������\"\n";
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

void LLparser::error()//���� �Ұ� ���� ���� �Լ�
{
	isError = true;
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::left_paren()//<left_paren> �� (                                    
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::right_paren()//<right_paren> �� )                                 
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::mult_op()//<mult_operator> �� * | /                                
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void LLparser::add_op()//<add_operator> �� + | -      
{
	sentenceContainer.push_back(lex.getTokenString());
	opCount++;
}

void LLparser::semi_colon()//<semi_colon> �� ;                                  
{
	sentenceContainer.push_back(lex.getTokenString());
	lex.lexical();
	if (lex.getTokenString() == ";")//�����ݷ� �ߺ� Ȯ��
	{
		isSemiColonRedundant = true;
		lex.lexical();
	}
	printSubResult();
}

void LLparser::assignment_op()//<assignment_op> �� :=                       
{
	sentenceContainer.push_back(lex.getTokenString());
}

void LLparser::ident()//<ident> �� any names conforming to C identifier rules    
{
	sentenceContainer.push_back(lex.getTokenString());
	idCount++;
}

void LLparser::constant()//<const> �� any decimal numbers                       
{
	sentenceContainer.push_back(lex.getTokenString());
	constCount++;
}

float LLparser::factor()//<factor> �� <left_paren><expression><right_paren> | <ident> | <const>
{
	float v1;
	switch (lex.getNextToken())
	{
	case LEFT_P:
		left_paren();
		lex.lexical();
		//�� ��ȣ �ߺ� �� ���� �� boolean���� ���� �� �Ѱܼ� ������ subresult �� �Ұ��� ���� warning log ��� //OK �� error �����ؼ��� radiobuttonó�� bool�� �ʿ�
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
		if (symbolTable.find(lex.getTokenString()) != symbolTable.end())//������ table�� �����ϴ� ���
		{
			return symbolTable[lex.getTokenString()];
		}
		else//������ table�� �������� �ʴ� ���
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

float LLparser::factor_tail(float beforeFactor)//<factor_tail> �� <mult_op><factor><factor_tail> | ��
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case MUL_OP :
		mult_op();
		op = lex.getTokenString();
		lex.lexical();
		if (op == lex.getTokenString())//���������� �ߺ� Ȯ��
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

float LLparser::term()//<term> �� <factor> <factor_tail>
{
	float temp;
	float v1 = factor();
	lex.lexical();
	float v2 = factor_tail(v1);
	return v2;
}

float LLparser::term_tail(float beforeTerm)//<term_tail> �� <add_op><term><term_tail> | ��
{
	string op;
	float v1, result;
	switch (lex.getNextToken())
	{
	case ADD_OP :
		add_op();
		op = lex.getTokenString();
		lex.lexical();
		if (op == lex.getTokenString())//�������� �ߺ� Ȯ��
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

float LLparser::expression()//<expression> �� <term><term_tail>
{
	float v1 = term();
	float v2 = term_tail(v1);
	return v2;
}

void LLparser::statement()//<statement> �� <ident><assignment_op><expression>
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
			if(!exist)//���� ������ ���� ������ ������ ���� ����Ͽ� �̹� �տ��� ������ ������ �ƴ� ���ο� ������ ��쿡�� ������ �߰�
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

void LLparser::statements()//<statements> �� <statement> | <statement><semi_colon><statements>
{
	statement();

	switch (lex.getNextToken())
	{
	case SEMI_COLON : 
		if (lex.checkNextTokenString() == "$") //������ �����ε� �����ݷ��� �پ��ִ� ���
		{
			isSemiColonWrongPlace = true;
			return;
		}
		semi_colon();
		//lex.lexical();   //�����ݷи� ����ó�� ������ semi_colon �Լ� ���ο� lexical�Լ��� �־���.
		statements();
		break;
	case END:
		break;
	default :
		error();
		break;
	}
}

void LLparser::program()//<program> �� <statements>
{
	lex.lexical();//initial lexical before start parsing
	statements();
	//Ʈ�� ��ȸ ��
	printSubResult();
	printFinalResult();
}