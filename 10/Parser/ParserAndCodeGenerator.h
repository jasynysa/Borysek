#pragma once
#include <fstream>
#include<string>
#include <vector>
#include"Tokenizer.h"
#include "Token.h"
#include"CodeGenerator.h"
#include"NameMenager.h"
using namespace std;
class ParserAndCodeGenerator
{
public:
	ParserAndCodeGenerator(Tokenizer& tokenizer, string scierzkaWyjscia) :
		tokenizer(tokenizer),
		codeGenerator(scierzkaWyjscia,nameMenager)
	{
		try
		{
			parsingAndCodeGenerating();
		}
		catch (const std::exception& e)
		{
			cout<<e.what();
		}
		codeGenerator.writeToFile();
	}
private:
	Tokenizer& tokenizer;
	CodeGenerator codeGenerator;
	NameMenager::NameMenager nameMenager;


	inline void parsingAndCodeGenerating()
	{
		parseClassAndGenerateCode();
	}

	//'class' className '{' classVarDec* subroutineDec* '}'
	void parseClassAndGenerateCode()
	{
		//'class' className '{'
		if (tokenizer.isItHasTokens({ clasS,identifier,curlyL }))
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
			if (!tokenizer.isItHasTokens(vector<EnumToken>{curlyR }))
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
		if (!tokenizer.isItHasTokens([](EnumToken t) {return t == statiC || t == field; }))
		{
			return false;
		}
		//type varName
		else if (tokenizer.isItHasTokens({ isType,[](EnumToken t) {return t == identifier; } }))
		{
			Token typ = tokenizer.getToken(-2);
			Token kind = tokenizer.getToken(-3);
			//(',' varName)*
			do
			{
				if (!nameMenager.isItVarNewName(tokenizer.getToken(-1), kind))
				{
					throw exception(((string)"variable identifier '"+tokenizer.getToken(-1).value +"' already exist").c_str());
				}
				//deklaracja zmiennej
				codeGenerator.classVariableDefinition(typ, tokenizer.getValue(-1));
			} while (tokenizer.isItHasTokens({ comma,identifier }));
			//';'
			if (!tokenizer.isItHasTokens(vector<EnumToken>{ EnumToken::semicolon }))
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
		if (!tokenizer.isItHasTokens([](EnumToken t) {return t == constructor || t == function || t == method ? true : false; }))
		{
			return false;
		}
		//('void' | type) subroutineName  
		if (!tokenizer.isItHasTokens({ [](EnumToken t) {return t == voiD || t == isType(t) ? true : false; },[](EnumToken t) {return t == identifier; } }))
		{			
			throw exception("wrong soubroutine definition");
		}
		Token subroutineName = tokenizer.getToken(-1);
		if (!nameMenager.isItSubroutineNewName(subroutineName, tokenizer.getToken(-3)))
			{
				throw exception(((string)"subroutine identifier '" + subroutineName.value + "' already exist").c_str());
			}
		codeGenerator.classSoubroutineDefinitionStart(tokenizer.getToken(-2), tokenizer.getToken(-1));
		
		//parameterList
		if (!parseParameterListAndGenerateCode(subroutineName))
		{
			throw exception("parameter list expected");
		}
		//subroutineBody
		if (!parseSubroutineBodyAndGenerateCode(subroutineName))
		{
			throw exception("sobroutine body expected");
		}
		codeGenerator.classSoubroutineDefinitionEnd();

		return true;
	}

	//'(' ( type varName (',' type varName)* )? ')'
	bool parseParameterListAndGenerateCode(const Token& subroutineName)
	{
		// '(' type varName
		if (!tokenizer.isItHasTokens(roundL))
		{
			return false;
		}
		codeGenerator.parameterListStart();

		if (tokenizer.isItHasTokens({ isType, [](EnumToken t) {return t==identifier; } }))
		{
			do
			{
				if (!nameMenager.isItSubroutinesVarNewName(subroutineName, tokenizer.getToken(-1), parameter))
				{
					throw exception(((string)"subroutine's variable identifier '" + tokenizer.getToken(-1).value + "' already exist").c_str());
				}
				codeGenerator.parameterDefinition(tokenizer.getToken(-2), tokenizer.getToken(-1));

			//(',' type varName)*
			} while (tokenizer.isItHasTokens({ [](EnumToken t) {return t == comma; },isType,[](EnumToken t) {return t == identifier; } }));
		}
		// ')'
		if (!tokenizer.isItHasTokens(roundR))
		{
			throw exception("expected ')'");
		}
		codeGenerator.parameterListEnd();

		return true;
	}

	//'{' varDec* statement* '}'
	bool parseSubroutineBodyAndGenerateCode(const Token& subroutineName)
	{
		//'{'
		if (!tokenizer.isItHasTokens(curlyL))
		{
			return false;
		}
		codeGenerator.subroutineBodyStart();

		//varDec*
		while (parseVariableDefinitionAndGenerateCode(subroutineName)) {}

		//statement*
		while (parseStatemantAndGenerateCode()) {}

		//'}'
		if (!tokenizer.isItHasTokens(curlyR))
		{
			throw exception("expected '}'");
		}
		codeGenerator.subroutineBodyEnd();

		return true;
	}

	//'var' type varName (',' varName)* ';'
	bool parseVariableDefinitionAndGenerateCode(const Token& subroutineName)
	{
		//'var'
		if (!tokenizer.isItHasTokens(var))
		{
			return false;
		}
		//type varName
		if (!(tokenizer.isItHasTokens(isType) && tokenizer.isItHasTokens(identifier)))
		{
			throw exception("wrong variable Definition");
		}
		do
		{
			if (!nameMenager.isItSubroutinesVarNewName(subroutineName, tokenizer.getToken(-1), local))
			{
				throw exception(((string)"subroutine's variable identifier '" + tokenizer.getToken(-1).value + "' already exist").c_str());
			}
			codeGenerator.subrutineVariableDefinition(tokenizer.getToken(-2), tokenizer.getToken(-1));

			//(',' varName)*
		} while (tokenizer.isItHasTokens({ comma,identifier }));

		//';'
		if (!tokenizer.isItHasTokens(semicolon))
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
		// 'let'
		if (!tokenizer.isItHasTokens(leT))
		{
			return false;
		}
		codeGenerator.letStatemantStart();
		// varName
		if (!tokenizer.isItHasTokens(identifier))
		{
			throw exception("wrong let statemant");
		}

		codeGenerator.letVariableStart(tokenizer.getToken(-1));

		// ('[' expression ']') ?
		if (tokenizer.isItHasTokens(squareL))
		{
			codeGenerator.letVariableIndexStart();

			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expretion required");
			}
			if (!tokenizer.isItHasTokens(squareR))
			{
				throw exception("expected ']'");
			}
			codeGenerator.letVariableIndexEnd();
		}
		codeGenerator.letVariableEnd();
		//'='
		if (!tokenizer.isItHasTokens(EnumToken::equal))
		{
			throw exception("expected '='");
		}
		// expression
		if (!parseExpressionAndGenerateCode())
		{
			throw exception("expecterd expresion");
		}		
		// ';'
		if (!tokenizer.isItHasTokens(semicolon))
		{
			throw exception("expected ';'");
		}
		codeGenerator.letStatemantEnd();

		return true;
	}
	//'if' '(' expression ')' '{' statements '}'
	//('else' '{' statements '}') ?
	bool parseIfStatemantAndGenerateCode() //another style of 'if' structure
	{
		if (tokenizer.isItHasTokens(iF))
		{
			codeGenerator.ifStatemantStart();
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected '('");
			}
			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expected expression");
			}
			if (!tokenizer.isItHasTokens(roundR))
			{
				throw exception("expected ')'");
			}
			if (!tokenizer.isItHasTokens(curlyL))
			{
				throw exception("expected '{'");
			}
			if (!parseStatemantAndGenerateCode())
			{
				throw exception("expected statemant");
			}
			if (!tokenizer.isItHasTokens(curlyR))
			{
				throw exception("expected '}'");
			}
			if (tokenizer.isItHasTokens(elsE))
			{
				if (!tokenizer.isItHasTokens(curlyL))
				{
					throw exception("expected '{'");
				}
				if (!parseStatemantAndGenerateCode())
				{
					throw exception("expected statemant");
				}
				if (!tokenizer.isItHasTokens(curlyR))
				{
					throw exception("expected '}'");
				}
			}
			codeGenerator.ifStatemantEnd();
			return true;
		}
		return false;
	}
	// 'while' '(' expression ')' '{' statements '}'
	bool parseWhileStatemantAndGenerateCode()
	{
		if (tokenizer.isItHasTokens(whilE))
		{
			codeGenerator.whileStatemantStart();
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected '('");
			}
			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expected expression");
			}
			if (!tokenizer.isItHasTokens(roundR))
			{
				throw exception("expected ')'");
			}
			if (!tokenizer.isItHasTokens(curlyL))
			{
				throw exception("expected '{'");
			}
			if (!parseStatemantAndGenerateCode())
			{
				throw exception("expected statemant");
			}
			if (!tokenizer.isItHasTokens(curlyR))
			{
				throw exception("expected '}'");
			}
			codeGenerator.whileStatemantEnd();
			return true;
		}
		return false;
	}
	// 'do' subroutineCall ';'
	bool parseDoStatemantAndGenerateCode()
	{
		if (tokenizer.isItHasTokens(dO))
		{
			codeGenerator.doStatemantStart();
			if (!parseSubroutineCallAndGenerateCode())
			{
				throw exception("expected subroutineCall");
			}
			if (!tokenizer.isItHasTokens(semicolon))
			{
				throw exception("expected ';'");
			}
			codeGenerator.doStatemantEnd();
			return true;
		}
		return false;
	}
	// 'return' expression? ';'
	bool parseReturnStatemantAndGenerateCode()
	{
		if (tokenizer.isItHasTokens(returN))
		{
			codeGenerator.returnStatemantStart();
			parseExpressionAndGenerateCode();
			if (!tokenizer.isItHasTokens(semicolon))
			{
				throw exception("expected ';'");
			}
			codeGenerator.returnStatemantEnd();
			return true;
		}
		return false;
	}

	//term (op term)*
	bool parseExpressionAndGenerateCode()
	{	
		//saving state of CG prewent from incorect code generating during parsing proces which in the end ocure incorect
		//saving state of tokenizer alowe to restor palce where parsing prooces begun
		auto codeGeneratorState = codeGenerator.save(); 
		auto tokenizerState = tokenizer.save();
		codeGenerator.expretionStart();	
		//term
		if (!parseTermAndGenerateCode())
		{
			codeGenerator.restore(codeGeneratorState);
			tokenizer.restore(tokenizerState);
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
		codeGenerator.expretionEnd();
		return true;
	}
	// integerConstant | stringConstant | keywordConstant |	
	// varName | varName '[' expression ']' | subroutineCall |
	//	'(' expression ')' | unaryOp term
	bool parseTermAndGenerateCode()
	{
		// integerConstant | stringConstant
		if (tokenizer.isItHasTokens([](EnumToken t){return t==integerConstant||t==stringConstant; }))
		{
			codeGenerator.constant(tokenizer.getToken(-1));
			return true;
		}
		// keywordConstant -> 'true'|'false'|'null'|'this'
		else if (tokenizer.isItHasTokens([](EnumToken t){return  t==truE||t==falsE||t==nulL||t==thiS; }))
		{
			codeGenerator.keywordConstant(tokenizer.getToken(-1));
			return true;
		}
		// subroutineCall
		else if (parseSubroutineCallAndGenerateCode())
		{
			return true;
		}
		//varName '[' expression ']'
		else if (tokenizer.isItHasTokens({ identifier, squareL }))
		{
			codeGenerator.arrayStart(tokenizer.getToken(-2));

			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expected expression");
			}
			if (!tokenizer.isItHasTokens(squareR))
			{
				throw exception("expected ']'");
			}
			codeGenerator.arrayEnd();
			return true;
		}
		//varName 
		else if (tokenizer.isItHasTokens(identifier))
		{
			codeGenerator.variabel(tokenizer.getToken(-1));
			return true;
		}		
		// '(' expression ')'
		else if (tokenizer.isItHasTokens(roundL))
		{
			if (!parseExpressionAndGenerateCode())
			{
				throw exception("expected expression");
			}
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected ')'");
			}
			return true;
		}
		//unaryOp term 
		//unaryOp-> '-' | '~'
		else if (tokenizer.isItHasTokens({ EnumToken::minus,tylda }))
		{
			codeGenerator.unaryOperator(tokenizer.getToken(-1));
			if (!parseTermAndGenerateCode())
			{
				throw exception("expected term");
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	//'+'|'-'|'*'|'/'|'&'|'|'|'<'|'>'|'='
	bool parseOpAndGenerateCode()
	{
		if (tokenizer.isItHasTokens([](EnumToken t) {return (t == EnumToken::plus || t == EnumToken::minus || t == star || t == slash || t == ampersand || t == line || t == angleL || t == angleR || t == EnumToken::equal); }))
		{
			codeGenerator.operatoR(tokenizer.getToken(-1));
			return true;
		}
		return false;
	}
	// subroutineName '(' expressionList ')' | 
	// (className |varName) '.' subroutineName '(' expressionList ')'
	bool parseSubroutineCallAndGenerateCode()
	{

		// subroutineName '('		//this combinationa of tokens clearly express subrutine call
		if (tokenizer.isItHasTokens({ identifier,roundL }))
		{
			codeGenerator.subroutineCallStart();
			codeGenerator.soubroutineName(tokenizer.getToken(-2));
			// expressionList 
			if (!parseExpressionListAndGenerateCode())
			{
				throw exception("expected expresion list");
			}
			//')'
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected ')'");
			}
		}
		//(className |varName) '.' subroutineName '(' expressionList ')' //this combinationa of tokens clearly express some class subrutine call
		else if (tokenizer.isItHasTokens({ identifier, dot,identifier,roundL }))
		{
			codeGenerator.subroutineCallStart();
			codeGenerator.soubroutineClassName(tokenizer.getToken(-4), tokenizer.getToken(-2));
			// expressionList
			if (!parseExpressionListAndGenerateCode())
			{
				throw exception("expected expresion list");
			}
			//')'
			if (!tokenizer.isItHasTokens(roundR))
			{
				throw exception("expected ')'");
			}
		}
		else
		{
			return false;
		}

		codeGenerator.subroutineCallEnd();
		return true;
	}
	// '(' (expression (',' expression)* )? ')'
	bool parseExpressionListAndGenerateCode()
	{
		codeGenerator.expressionListStart();
		if (parseExpressionAndGenerateCode())
		{
			while (tokenizer.isItHasTokens(comma))
			{
				parseExpressionAndGenerateCode();
			}
		}
		codeGenerator.expressionListEnd();
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