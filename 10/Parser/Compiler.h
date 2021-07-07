#pragma once
#include<string>
#include"Tokenizer.h"
#include"ParserAndCodeGenerator.h"
#include"CodeGenerator.h"
using namespace std;
class Compiler
{
public:

	Compiler(string sciezkaZrodla, string scierzkaWyjscia ):
	tokenizer(sciezkaZrodla),
	parserAndCodeGenerator(tokenizer, scierzkaWyjscia)
	{}

private:
	Tokenizer tokenizer;
	ParserAndCodeGenerator parserAndCodeGenerator;
};

