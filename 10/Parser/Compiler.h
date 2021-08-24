#pragma once
#include<string>
#include"Tokenizer.h"
#include"Parser.h"
#include"CodeGenerator.h"
using namespace std;
class Compiler
{
public:

	Compiler(string sciezkaZrodla, string scierzkaWyjscia ):
	tokenizer(sciezkaZrodla),
	parser(tokenizer),
	codeGenerator(parser.getSyntaxTree(), scierzkaWyjscia)
	{}

private:
	Tokenizer tokenizer;
	Parser parser;
	CodeGenerator codeGenerator;
};

