#pragma once
#include <fstream>
#include<string>
#include <vector>
#include"Tokenizer.h"
#include "Token.h"
#include"CodeGenerator.h"
using namespace std;
class ParserAndCodeGenerator
{
public:
	ParserAndCodeGenerator(Tokenizer& tokenizer, string scierzkaWyjscia) :
		tokenizer(tokenizer),
		codeGenerator(scierzkaWyjscia)
	{
		try
		{
			parsingAndCodeGenerating();
		}
		catch (const std::exception& e)
		{
			cout<<e.what();
		}

	}

private:
	Tokenizer& tokenizer;
	CodeGenerator codeGenerator;


	inline void parsingAndCodeGenerating()
	{
		parseClassAndGenerateCode();
	}

	//'class' className '{' classVarDec* subroutineDec* '}'
	void parseClassAndGenerateCode()
	{
		//'class' className '{'
		if (tokenizer.hasTokens({ clasS,identifier,curlyL }))
		{
			//rozpoczecie definicji kalsy
			codeGenerator.classDefinitionStart(tokenizer.getValue(-2));
			//classVarDec*
			while (parseClassVariableDefinitionAndGenerateCode()) {}
			//subroutineDec*
			while (parseClassSubroutineDefinitionAndGenerateCode()) {}
			//zakonczenie definicji klasy
			codeGenerator.classDefinitionEnd();
			//'}'
			if (!tokenizer.hasTokens(vector<EnumToken>{curlyR }))
			{
				throw exception("lack of close bracket");
			}
		}
		else
		{
			throw exception("wrong class definition");
		}
	}

	//('static' | 'field') type varName(',' varName) * ';'
	bool parseClassVariableDefinitionAndGenerateCode()
	{
		//('static' | 'field')
		if (!tokenizer.hasTokens([](EnumToken t) {return t == statiC || t == field ? true : false; }))
		{
			return false;
		}
		//type varName
		else if (tokenizer.hasTokens({ isType,[](EnumToken t) {return t == identifier ? true : false; } }))
		{
			Token typ = tokenizer.getToken(-2);
			//(',' varName)*
			do
			{
				//deklaracja zmiennej
				codeGenerator.classVariableDefinition(typ, tokenizer.getValue(-1));
			} while (tokenizer.hasTokens({ comma,identifier }));
			//';'
			if (!tokenizer.hasTokens(vector<EnumToken>{ EnumToken::semicolon }))
			{
				throw exception("brak srednika");
			}
			return true;
		}
		else
		{
			throw exception("nieprawidlowa definicja zmiennej");
		}

	}

	//('constructor' | 'function' | 'method') ('void' | type) subroutineName parameterList subroutineBody
	bool parseClassSubroutineDefinitionAndGenerateCode()
	{
		//('constructor' | 'function' | 'method')
		if (!tokenizer.hasTokens([](EnumToken t) {return t == constructor || t == function || t == method ? true : false; }))
		{
			return false;
		}
		//('void' | type) subroutineName  
		if (!tokenizer.hasTokens({ [](EnumToken t) {return t == voiD || t == isType(t) ? true : false; },[](EnumToken t) {return t == identifier; } }))
		{
			throw exception("wrong soubroutine definition");
		}
		codeGenerator.classSoubroutineDefinitionStart(tokenizer.getToken(-2), tokenizer.getToken(-1));

		//parameterList
		if (!parseParameterListAndGenerateCode())
		{
			throw exception("parameter list expected");
		}
		//subroutineBody
		if (!parseSubroutineBodyAndGenerateCode())
		{
			throw exception("sobroutine body expected");
		}
		codeGenerator.classSoubroutineDefinitionEnd();

		return true;
	}

	//'(' ( type varName (',' type varName)* )? ')'
	bool parseParameterListAndGenerateCode()
	{
		// '(' type varName
		if (!tokenizer.hasTokens(roundL))
		{
			return false;
		}
		codeGenerator.parameterListStart();

		if (tokenizer.hasTokens(isType) && tokenizer.hasTokens(identifier))
		{
			do
			{
				codeGenerator.parameterDefinition(tokenizer.getToken(-2), tokenizer.getToken(-1));

			//(',' type varName)*
			} while (tokenizer.hasTokens(comma) && tokenizer.hasTokens(isType) && tokenizer.hasTokens(identifier));
		}
		// ')'
		if (!tokenizer.hasTokens(roundR))
		{
			throw exception("expected ')'");
		}
		codeGenerator.parameterListEnd();

		return true;
	}

	//'{' varDec* statement* '}'
	bool parseSubroutineBodyAndGenerateCode()
	{
		//'{'
		if (!tokenizer.hasTokens(curlyL))
		{
			return false;
		}
		codeGenerator.subroutineBodyStart();

		//varDec*
		while (parseVariableDefinitionAndGenerateCode()) {}

		//statement*
		while (parseStatemantAndGenerateCode()) {}

		//'}'
		if (!tokenizer.hasTokens(curlyR))
		{
			throw exception("expected '}'");
		}
		codeGenerator.subroutineBodyEnd();

		return true;
	}

	//'var' type varName (',' varName)* ';'
	bool parseVariableDefinitionAndGenerateCode()
	{
		//'var'
		if (!tokenizer.hasTokens(var))
		{
			return false;
		}
		//type varName
		if (!(tokenizer.hasTokens(isType) && tokenizer.hasTokens(identifier)))
		{
			throw exception("wrong variable Definition");
		}
		do
		{
			//codeGenerator.variableDefinition();

			//(',' varName)*
		} while (tokenizer.hasTokens({ comma,identifier }));

		//';'
		if (!tokenizer.hasTokens(semicolon))
		{
			throw exception("expected ';'");
		}
		return true;
	}

	//letStatement | ifStatement | whileStatement |	doStatement | returnStatement
	bool parseStatemantAndGenerateCode()
	{
		if (parseLetStatemantAndGenerateCode())
		{
			return true;
		}
		else if (parseIfStatemantAndGenerateCode())
		{
			return true;
		}
		else if (parseWhileStatemantAndGenerateCode())
		{
			return true;
		}
		else if (parseDoStatemantAndGenerateCode())
		{
			return true;
		}
		else if (parseReturnStatemantAndGenerateCode())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//'let' varName('[' expression ']') ? '=' expression ';'
	bool parseLetStatemantAndGenerateCode()
	{
		codeGenerator.letStatemantStart();

		// 'let'
		if (!tokenizer.hasTokens(leT))
		{
			return false;
		}
		// varName
		if (!tokenizer.hasTokens(identifier))
		{
			throw exception("wrong let statemant");
		}

		codeGenerator.letVariableStart(tokenizer.getToken(-1));

		// ('[' expression ']') ?
		if (tokenizer.hasTokens(squareL))
		{
			codeGenerator.letVariableIndexStart();

			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expretion required");
			}
			if (!tokenizer.hasTokens(squareR))
			{
				throw exception("expected ']'");
			}
			codeGenerator.letVariableIndexEnd();
		}
		codeGenerator.letVariableEnd();
		//'='
		if (!tokenizer.hasTokens(EnumToken::equal))
		{
			throw exception("expected '='");
		}
		// expression
		if (!parseExpressionAndGenerateCode())
		{
			throw exception("expected expresion");
		}
		// ';'
		if (!tokenizer.hasTokens(semicolon))
		{
			throw exception("expected ';'");
		}
		codeGenerator.letStatemantEnd();

		return true;
	}

	bool parseIfStatemantAndGenerateCode()
	{
		return true;
	}

	bool parseWhileStatemantAndGenerateCode()
	{
		return true;
	}

	bool parseDoStatemantAndGenerateCode()
	{
		return true;
	}

	bool parseReturnStatemantAndGenerateCode()
	{
		return true;
	}

	//term (op term)*
	bool parseExpressionAndGenerateCode()
	{
		//term
		if (!parseTermAndGenerateCode())
		{
			return false;
		}

		//(op term)*
		while (parseOpAndGenerateCode())
		{
			if (!parseTermAndGenerateCode())
			{
				throw exception("expected term");
			}
		}
		return true;
	}

	bool parseTermAndGenerateCode()
	{

		return true;
	}

	bool parseOpAndGenerateCode()
	{
		return true;
	}

	static bool isType(EnumToken t)
	{
		if (t == inT || t == chaR || t == boolean || t == identifier)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};