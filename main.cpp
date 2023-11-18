#include "LLparser.h"

int main(int argc, char* argv[])
{
	LLparser parser;
	const string option = "-v";
	if (!option.compare(argv[1]))
	{
		parser.lex.loadFileData(argv[2]);
		parser.lex.tokenize();
		parser.lex.printAllTokens();
	}	
	else
	{
		parser.lex.loadFileData(argv[1]);
		parser.lex.tokenize();
		parser.program();
	}
}