#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "Tokenizer.h"
#include "Token.h"
#include "CodeGenerator.h"
#include "SyntaxTree.h"
#include <sstream> 
using namespace std;
class Parser
{
public:
	Parser(Tokenizer& tokenizer) :
		tokenizer(tokenizer)
	{
		try
		{
			parsingAndCodeGenerating();
		}
		catch (const std::exception& e)
		{
			cout << e.what();
		}
	}

	SyntaxTree& getSyntaxTree()
	{
		return syntaxTree;
	}
private:
	Tokenizer& tokenizer;
	SyntaxTree syntaxTree;


	inline void parsingAndCodeGenerating()
	{
		parseClass();
	}

	//'class' className '{' classVarDec* subroutineDec* '}'
	void parseClass()
	{
		//'class' className '{'
		if (tokenizer.isItHasTokens({ EnumToken::clasS,identifier,curlyL }))
		{
			SyntaxTree::NodePointer parentClass = syntaxTree.addNode({ tokenizer.getToken(-3), tokenizer.getToken(-2) }, "classDefinition");

			SyntaxTree::NodePointer classVarsDefinitions = syntaxTree.addNode({}, parentClass, "classVariablesDefinition");
			//classVarDef*
			while (parseClassVariableDefinition(classVarsDefinitions)) {}

			SyntaxTree::NodePointer classSubroutinesDefinitions = syntaxTree.addNode({}, parentClass,"calssSubroutinesDefinition" );
			//subroutineDef*
			while (parseClassSubroutineDefinition(classSubroutinesDefinitions)) {}

			//'}'
			if (!tokenizer.isItHasTokens(vector<EnumToken>{curlyR }))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "  lack of close bracket";
				throw exception(s.str().c_str());
			}
		}
		else
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " wrong class definition";
			throw exception(s.str().c_str());
		}

	}

	//('static' | 'field') type varName(',' varName) * ';'
	bool parseClassVariableDefinition(SyntaxTree::NodePointer classVariablesDefinitions)
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
				if (syntaxTree.isItVarName(tokenizer.getToken(-1)))
				{
					stringstream s;
					s << "line: " << tokenizer.getToken(-1).nrLini << " variable identifier '" << tokenizer.getToken(-1).value << "' already exist";
					throw exception(s.str().c_str());
				}
				//definicja zmiennej
				syntaxTree.addNode({ kind,typ,tokenizer.getToken(-1) }, classVariablesDefinitions,"classVabiableDefinition");
			} while (tokenizer.isItHasTokens({ comma,identifier }));
			//';'
			if (!tokenizer.isItHasTokens(vector<EnumToken>{ EnumToken::semicolon }))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "  lack of close semicolon";
				throw exception(s.str().c_str());
			}
			return true;
		}
		else
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " wrong wariable definition";
			throw exception(s.str().c_str());
		}

	}

	//('constructor' | 'function' | 'method') ('void' | type) subroutineName parameterList subroutineBody
	bool parseClassSubroutineDefinition(SyntaxTree::NodePointer classSubroutinesDefinitions)
	{
		//('constructor' | 'function' | 'method')
		if (!tokenizer.isItHasTokens([](EnumToken t) {return t == constructor || t == function || t == method ? true : false; }))
		{
			return false;
		}
		//('void' | type) subroutineName  
		if (!tokenizer.isItHasTokens({ [](EnumToken t) {return t == voiD || t == isType(t); },[](EnumToken t) {return t == identifier; } }))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " wrong soubroutine definition";
			throw exception(s.str().c_str());
		}
		Token subroutineName = tokenizer.getToken(-1);
		if (syntaxTree.isItSubroutinesName(subroutineName))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "subroutine identifier '" << subroutineName.value << "' already exist";
			throw exception(s.str().c_str());
		}
		//codeGenerator.classSoubroutineDefinitionStart(tokenizer.getToken(-2), tokenizer.getToken(-1));
		SyntaxTree::NodePointer subroutine = syntaxTree.addNode({ tokenizer.getToken(-3), tokenizer.getToken(-2), tokenizer.getToken(-1) }, classSubroutinesDefinitions, "calssSubroutineDefinition");

		//parameterList
		if (!parseParameterList(subroutine))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " parameter list expected";
			throw exception(s.str().c_str());
		}
		//subroutineBody
		if (!parseSubroutineBody(subroutine))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " sobroutine body expected";
			throw exception(s.str().c_str());
		}
		//codeGenerator.classSoubroutineDefinitionEnd();

		return true;
	}

	//'(' ( type varName (',' type varName)* )? ')'
	bool parseParameterList(SyntaxTree::NodePointer subroutine)
	{
		// '(' type varName
		if (!tokenizer.isItHasTokens(roundL))
		{
			return false;
		}
		SyntaxTree::NodePointer parameterList = syntaxTree.addNode({}, subroutine, "parameterList");
		if (tokenizer.isItHasTokens({ isType, [](EnumToken t) {return t == identifier; } }))
		{
			do
			{
				if (syntaxTree.isItSubroutinesVariableName(tokenizer.getToken(-1), subroutine))
				{
					stringstream s;
					s << "line: " << tokenizer.getToken(-1).nrLini << "subroutine's variable identifier '" << tokenizer.getToken(-1).value << "' already exist";
					throw exception(s.str().c_str());
				}
				//codeGenerator.parameterDefinition(tokenizer.getToken(-2), tokenizer.getToken(-1));
				syntaxTree.addNode({ tokenizer.getToken(-2),tokenizer.getToken(-1) }, parameterList, "parameterDefinition");

				//(',' type varName)*
			} while (tokenizer.isItHasTokens({ [](EnumToken t) {return t == comma; },isType,[](EnumToken t) {return t == identifier; } }));
		}
		// ')'
		if (!tokenizer.isItHasTokens(roundR))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " expected ')'";
			throw exception(s.str().c_str());
		}

		return true;
	}

	//'{' varDec* statement* '}'
	bool parseSubroutineBody(SyntaxTree::NodePointer subroutine)
	{
		//'{'
		if (!tokenizer.isItHasTokens(curlyL))
		{
			return false;
		}
		SyntaxTree::NodePointer subroutineBody = syntaxTree.addNode({}, subroutine, "subroutineBody");

		SyntaxTree::NodePointer localVarsDefinitions = syntaxTree.addNode({}, subroutineBody,"localVariablesDefinitions");
		//varDec*
		while (parseSubroutinesVariablesDefinition(localVarsDefinitions)) {}

		SyntaxTree::NodePointer statemants = syntaxTree.addNode({}, subroutineBody,"statmants");
		//statement*
		while (parseStatemant(statemants)) {}

		//'}'
		if (!tokenizer.isItHasTokens(curlyR))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " expected '}'";
			throw exception(s.str().c_str());
		}
		return true;
	}

	//'var' type varName (',' varName)* ';'
	bool parseSubroutinesVariablesDefinition(SyntaxTree::NodePointer localVariablesDefinitions)
	{
		//'var'
		if (!tokenizer.isItHasTokens(var))
		{
			return false;
		}
		//type varName
		if (!(tokenizer.isItHasTokens(isType) && tokenizer.isItHasTokens(identifier)))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << " wrong variable Definition";
			throw exception(s.str().c_str());
		}
		do
		{
			if (syntaxTree.isItSubroutinesVariableName(tokenizer.getToken(-1), localVariablesDefinitions->parent->parent))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "subroutine's variable identifier '" << tokenizer.getToken(-1).value << "' already exist";
				throw exception(s.str().c_str());
			}
			syntaxTree.addNode({ tokenizer.getToken(-2), tokenizer.getToken(-1) }, localVariablesDefinitions, "localVariableDefinition");

			//(',' varName)*
		} while (tokenizer.isItHasTokens({ comma,identifier }));

		//';'
		if (!tokenizer.isItHasTokens(semicolon))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "expected ';'";
			throw exception(s.str().c_str());
		}
		return true;
	}

	//letStatement | ifStatement | whileStatement |	doStatement | returnStatement
	bool parseStatemant(SyntaxTree::NodePointer statemants)
	{
		if (parseLetStatemant(statemants))
		{
			return true;
		}
		else if (parseIfStatemant(statemants))
		{
			return true;
		}
		else if (parseWhileStatemant(statemants))
		{
			return true;
		}
		else if (parseDoStatemant(statemants))
		{
			return true;
		}
		else if (parseReturnStatemant(statemants))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//'let' varSelect '=' expression ';'
	bool parseLetStatemant(SyntaxTree::NodePointer statemants)
	{
		// 'let'
		if (!tokenizer.isItHasTokens(leT))
		{
			return false;
		}
		SyntaxTree::NodePointer letStatemant = syntaxTree.addNode({ tokenizer.getToken(-1) }, statemants, "letStatemant");

		//varSelect
		if (!parseVarSelect(letStatemant))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "wrong let statemant";
			throw exception(s.str().c_str());
		}
		//'='
		if (!tokenizer.isItHasTokens(EnumToken::equal))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "expected '='";
			throw exception(s.str().c_str());
		}
		// expression
		if (!parseExpression(letStatemant))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "expected expresion";
			throw exception(s.str().c_str());
		}
		// ';'
		if (!tokenizer.isItHasTokens(semicolon))
		{
			stringstream s;
			s << "line: " << tokenizer.getToken(-1).nrLini << "expected ';'";
			throw exception(s.str().c_str());
		}

		return true;
	}

	//'if' '(' expression ')' '{' statements '}'
	//('else' '{' statements '}') ?
	bool parseIfStatemant(SyntaxTree::NodePointer statemants) //another style of 'if' structure
	{
		if (tokenizer.isItHasTokens(iF))
		{
			//codeGenerator.ifStatemantStart();
			SyntaxTree::NodePointer ifStatemant = syntaxTree.addNode({ tokenizer.getToken(-1) }, statemants, "ifStatemant");

			if (!tokenizer.isItHasTokens(roundL))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected ';'";
				throw exception(s.str().c_str());
				throw exception("expected '('");
			}
			if (!parseExpression(ifStatemant))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected expression";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(roundR))
			{
				throw exception("expected ')'");
			}
			if (!tokenizer.isItHasTokens(curlyL))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected '{'";
				throw exception(s.str().c_str());
			}
			if (!parseStatemant(ifStatemant))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected statemant";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(curlyR))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected '}'";
				throw exception(s.str().c_str());
			}
			if (tokenizer.isItHasTokens(elsE))
			{
				if (!tokenizer.isItHasTokens(curlyL))
				{
					stringstream s;
					s << "line: " << tokenizer.getToken(-1).nrLini << "expected '{'";
					throw exception(s.str().c_str());
				}
				if (!parseStatemant(ifStatemant))
				{
					stringstream s;
					s << "line: " << tokenizer.getToken(-1).nrLini << "expected statemant";
					throw exception(s.str().c_str());
				}
				if (!tokenizer.isItHasTokens(curlyR))
				{
					stringstream s;
					s << "line: " << tokenizer.getToken(-1).nrLini << "expected '}'";
					throw exception(s.str().c_str());
				}
			}
			return true;
		}
		return false;
	}

	// 'while' '(' expression ')' '{' statements '}'
	bool parseWhileStatemant(SyntaxTree::NodePointer statemants)
	{
		if (tokenizer.isItHasTokens(whilE))
		{
			SyntaxTree::NodePointer whileStatemant = syntaxTree.addNode({ tokenizer.getToken(-1) }, statemants, "whileStatemant");
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected '('");
			}
			if (!parseExpression(whileStatemant))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected expression";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(roundR))
			{
				throw exception("expected ')'");
			}
			if (!tokenizer.isItHasTokens(curlyL))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected '{'";
				throw exception(s.str().c_str());
			}
			if (!parseStatemant(whileStatemant))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected statemant";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(curlyR))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected '}'";
				throw exception(s.str().c_str());
			}
			return true;
		}
		return false;
	}

	// 'do' subroutineCall ';'
	bool parseDoStatemant(SyntaxTree::NodePointer statemants)
	{
		if (tokenizer.isItHasTokens(dO))
		{
			SyntaxTree::NodePointer doStatemant = syntaxTree.addNode({ tokenizer.getToken(-1) }, statemants, "doStatemant");

			if (!parseSubroutineCall(doStatemant))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected subroutineCall";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(semicolon))
			{
				throw exception("expected ';'");
			}
			return true;
		}
		return false;
	}

	// 'return' expression? ';'
	bool parseReturnStatemant(SyntaxTree::NodePointer statemants)
	{
		if (tokenizer.isItHasTokens(returN))
		{
			SyntaxTree::NodePointer returnStatemant = syntaxTree.addNode({ tokenizer.getToken(-1) }, statemants, "returnStatemant");

			parseExpression(returnStatemant);
			if (!tokenizer.isItHasTokens(semicolon))
			{
				throw exception("expected ';'");
			}
			return true;
		}
		return false;
	}

	//' varName ('[' expression ']')?
	bool parseVarSelect(SyntaxTree::NodePointer parent)
	{
		// varName
		if (!tokenizer.isItHasTokens(identifier))
		{
			return false;
		}
		SyntaxTree::NodePointer varSelect = syntaxTree.addNode({ tokenizer.getToken(-1) }, parent, "variableSelection");

		// ('[' expression ']') ?
		if (tokenizer.isItHasTokens(squareL))
		{
			if (!parseExpression(varSelect))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expretion required";
				throw exception(s.str().c_str());
			}
			if (!tokenizer.isItHasTokens(squareR))
			{
				throw exception("expected ']'");
			}
		}
		return true;
	}
	//term (op term)*
	bool parseExpression(SyntaxTree::NodePointer parent)
	{
		//saving state of CG prewent from incorect code generating during parsing proces which in the end ocure incorect
		//saving state of tokenizer alowe to restor palce where parsing prooces begun
		/*auto codeGeneratorState = codeGenerator.save();
		auto tokenizerState = tokenizer.save();
		codeGenerator.expretionStart();	*/
		SyntaxTree::NodePointer expretion = syntaxTree.addNode({}, parent, "expretion");
		//term
		if (!parseTerm(expretion))
		{
			return false;
		}

		//(op term)*
		while (parseOp(expretion))
		{

			if (!parseTerm(expretion->getChild(0)))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected term";
				throw exception(s.str().c_str());
			}
		}
		return true;
	}
	// integerConstant | stringConstant | keywordConstant |	
	// varName | varName '[' expression ']' | subroutineCall |
	//	'(' expression ')' | unaryOp term
	bool parseTerm(SyntaxTree::NodePointer parent)
	{
		// integerConstant | stringConstant
		if (tokenizer.isItHasTokens([](EnumToken t) {return t == integerConstant || t == stringConstant; }))
		{
			syntaxTree.addNode({ tokenizer.getToken(-1) }, parent, "integerConstant");
			return true;
		}
		// keywordConstant -> 'true'|'false'|'null'|'this'
		else if (tokenizer.isItHasTokens([](EnumToken t) {return  t == truE || t == falsE || t == nulL || t == thiS; }))
		{
			syntaxTree.addNode({ tokenizer.getToken(-1) }, parent,"keywordConstant");
			return true;
		}
		// subroutineCall
		else if (parseSubroutineCall(parent))
		{
			return true;
		}
		//varSelect
		else if (parseVarSelect(parent))
		{
			return true;
		}
		// '(' expression ')'
		else if (tokenizer.isItHasTokens(roundL))
		{
			if (!parseExpression(parent))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected expression";
				throw exception(s.str().c_str());
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
			//codeGenerator.unaryOperator(tokenizer.getToken(-1));
			SyntaxTree::NodePointer unaryOp = syntaxTree.addNode({ tokenizer.getToken(-1) }, parent, "unaryOperator");
			if (!parseTerm(unaryOp))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected term";
				throw exception(s.str().c_str());
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	//'+'|'-'|'*'|'/'|'&'|'|'|'<'|'>'|'='
	bool parseOp(SyntaxTree::NodePointer parent)
	{
		if (tokenizer.isItHasTokens([](EnumToken t) {return (t == EnumToken::plus || t == EnumToken::minus || t == star || t == slash || t == ampersand || t == line || t == angleL || t == angleR || t == EnumToken::equal); }))
		{
			syntaxTree.pushOver({ tokenizer.getToken(-1) }, parent->getChild(0), "operetor");
			return true;
		}
		return false;
	}
	// subroutineName '(' expressionList ')' | 
	// (className |varName) '.' subroutineName '(' expressionList ')'
	bool parseSubroutineCall(SyntaxTree::NodePointer parent)
	{

		// subroutineName '('		//this combinationa of tokens clearly express subrutine call
		if (tokenizer.isItHasTokens({ identifier,roundL }))
		{
			SyntaxTree::NodePointer subroutineCall = syntaxTree.addNode({ tokenizer.getToken(-2) }, parent, "subroutineCall");
			//codeGenerator.soubroutineName(tokenizer.getToken(-2));
			// expressionList 
			if (!parseExpressionList(subroutineCall))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected expresion list";
				throw exception(s.str().c_str());
			}
			//')'
			if (!tokenizer.isItHasTokens(roundL))
			{
				throw exception("expected ')'");
			}
		}
		//(className |varName) '.' subroutineName '(' expressionList ')' //this combinationa of tokens clearly express some class's subrutine call
		else if (tokenizer.isItHasTokens({ identifier, dot,identifier,roundL }))
		{
			//codeGenerator.subroutineCallStart();
			//codeGenerator.soubroutineClassName(tokenizer.getToken(-4), tokenizer.getToken(-2));
			SyntaxTree::NodePointer subroutineCall = syntaxTree.addNode({ tokenizer.getToken(-4), tokenizer.getToken(-2) }, parent,"calssSubroutineCall");
			// expressionList
			if (!parseExpressionList(subroutineCall))
			{
				stringstream s;
				s << "line: " << tokenizer.getToken(-1).nrLini << "expected expresion list";
				throw exception(s.str().c_str());
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
		return true;
	}
	// '(' (expression (',' expression)* )? ')'
	bool parseExpressionList(SyntaxTree::NodePointer subroutineCall)
	{
		if (parseExpression(subroutineCall))
		{
			while (tokenizer.isItHasTokens(comma))
			{
				parseExpression(subroutineCall);
			}
		}
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