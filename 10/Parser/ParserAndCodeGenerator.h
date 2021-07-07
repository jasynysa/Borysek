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
			codeGenerator.classDefinitionStart(tokenizer.getValue(-1));
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
		//poczatek deklaracji Subrutyny------------------------

		//parameterList
		if (!parseParamiterListAndGenerateCode())
		{
			throw exception("parameter list expected");
		}
		//subroutineBody
		if (!parseSubroutineBodyAndGenerateCode())
		{
			throw exception("sobroutine body expected");
		}
		//koniec dekalracji subrutyny---------------------------

		return true;
	}

	//'(' ( type varName (',' type varName)* )? ')'
	bool parseParamiterListAndGenerateCode()
	{
		// '(' type varName
		if (!tokenizer.hasTokens(roundL))
		{
			return false;
		}
		//codeGenerator.paramiterListStart();----------------

		if (tokenizer.hasTokens(isType) && tokenizer.hasTokens(identifier))
		{
			do
			{
				//codeGenerator.paramiterDefinition();----------------

			//(',' type varName)*
			} while (tokenizer.hasTokens(comma) && tokenizer.hasTokens(isType) && tokenizer.hasTokens(identifier));
		}
		// ')'
		if (!tokenizer.hasTokens(roundR))
		{
			throw exception("expected ')'");
		}
		//codeGenerator.paramiterListEnd();----------------------

		return true;
	}

	//'{' varDec* statements? '}'
	bool parseSubroutineBodyAndGenerateCode()
	{
		//'{'
		if (!tokenizer.hasTokens(curlyL))
		{
			return false;
		}
		//codeGenerator.subroutineBodyStart();

		//varDec*
		while (parseVariableDefinitionAndGenerateCode()) {}

		//statements?
		parseStatemantsAndGenerateCode();

		//'}'
		if (!tokenizer.hasTokens(curlyR))
		{
			throw exception("expected '}'");
		}
		//codeGenerator.subroutineBodyEnd();

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

	bool parseStatemantsAndGenerateCode()
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