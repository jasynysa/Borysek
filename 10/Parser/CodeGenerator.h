#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<iostream>
#include "Token.h"
using namespace std;
static class CodeGenerator
{
public:
	CodeGenerator(string scierzkazWyjscia, NameMenager::NameMenager nm):
		nameMenager(nm)
	{
		otwarciePliku(scierzkazWyjscia);


	}

	struct codeGeneratorState
	{
		string::iterator it;
		int glebokosc;
	};
	//save actual state of code 
	codeGeneratorState save()
	{
		codeGeneratorState cgs;
		cgs.glebokosc = glebokosc;
		cgs.it = code.end();
		return cgs;
	}
	//restore state of code from moment of call save function
	void restore(codeGeneratorState state)
	{
		code.erase(state.it, code.end());
		glebokosc = state.glebokosc;
	}
	void writeToFile()
	{
		plikWyjsciowy << code;
	}

	void classDefinitionStart(string identifier)
	{
		code += "<class>\n";
		 + "\t<keyword> class </keyword>\n";
		 + "\t<indentifier> " + identifier + "</identifier\n>";
		 + "\t<symbol> { </symbol\n>" ;
		glebokosc++;
	}
	void classDefinitionEnd()
	{
		glebokosc--;
		code += "<\class>\n";
	}
	void classVariableDefinition(const Token& type, string name)
	{
		code += tabulacja() + "<variableDefinition>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name + "<name>\n"
			+ tabulacja() + "</variableDefinition>\n";

	}
	void classSoubroutineDefinitionStart(Token returnType, Token name)
	{
		code += tabulacja() + "<soubroutineDefinition>\n";
		glebokosc++;
		code += tabulacja() + "<returnType>" + typeName(returnType) + "</returnType>\n"
			+ tabulacja() + "<name>" + name.value + "</name>\n";


	}
	void classSoubroutineDefinitionEnd()
	{
		glebokosc--;
		code += tabulacja() + "</soubroutineDefinition>\n";
	}
	void parameterListStart()
	{
		code += tabulacja() + "<parameterList>\n";
		glebokosc++;
	}
	void parameterDefinition(const Token& type, const Token& name)
	{
		code += tabulacja() + "<parameter>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</parameter>\n";
	}
	void parameterListEnd()
	{
		glebokosc--;
		code += tabulacja() + "</paremetreList>\n";
	}
	void subroutineBodyStart()
	{
		code += tabulacja() + "<subroutineBody>\n";
		glebokosc++;
	}
	void subroutineBodyEnd()
	{
		glebokosc--;
		code += tabulacja() + "</subroutineBody>\n";
	}
	void letStatemantStart()
	{
		code += tabulacja() + "<letStatemant>\n";
		glebokosc++;
	}
	void letStatemantEnd()
	{
		glebokosc--;
		code += tabulacja() + "</letStatemant>\n";
	}
	void ifStatemantStart()
	{
		code += tabulacja() + "<ifStatemant>\n";
		glebokosc++;
	}
	void ifStatemantEnd()
	{
		glebokosc--;
		code += tabulacja() + "</ifStatemant>\n";
	}
	void whileStatemantStart()
	{
		code += tabulacja() + "<whielStatemant>\n";
		glebokosc++;
	}
	void whileStatemantEnd()
	{
		glebokosc--;
		code += tabulacja() + "</whileStatemant>\n";
	}
	void doStatemantStart()
	{
		code += tabulacja() + "<doStatemant>\n";
		glebokosc++;

	}
	void doStatemantEnd()
	{
		glebokosc--;
		code += tabulacja() + "</doStatemant>\n";
	}
	void returnStatemantStart()
	{
		code += tabulacja() + "<returnStatemant>\n";
		glebokosc++;
	}
	void returnStatemantEnd()
	{
		glebokosc--;
		code += tabulacja() + "</returnStatemant>\n";
	}
	void letVariableStart(const Token& var)
	{
		code += tabulacja() + "<letVariable>\n";
		glebokosc++;
		code += tabulacja() + "<name>" + var.value + "</name>\n";
	}
	void letVariableEnd()
	{
		glebokosc--;
		code += tabulacja() + "</letVariable>\n";
	}
	void letVariableIndexStart()
	{
		code += tabulacja() + "<index>\n";
		glebokosc++;
	}
	void letVariableIndexEnd()
	{
		glebokosc--;
		code += tabulacja() + "</index>\n";
	}
	void expretionStart()
	{
		code += tabulacja() + "<expretion>\n";
		glebokosc++;
	}
	void expretionEnd()
	{
		glebokosc--;
		code += tabulacja() + "</expretion>\n";
	}
	void operatoR(const Token& op)
	{
		code += tabulacja() + "<operator>" + tokenyNazwa[op.token] + "</operator>\n";
	}
	void unaryOperator(const Token& op)
	{
		code += tabulacja() + "<unaryOperator>" + tokenyNazwa[op.token] + "</unaryOperator>\n";
	}
	void constant(const Token& constant)
	{
		code += tabulacja() + "<" + tokenyNazwa[constant.token] + ">\n"
			+ tabulacja() + "\t<value>" + constant.value + "</value>\n"
			+tabulacja()+"</" + tokenyNazwa[constant.token] + ">\n";
	}
	void keywordConstant(const Token& keyword)
	{
		code += tabulacja() + "<keywordConstant>" + tokenyNazwa[keyword.token] + "</keywordConstant>\n";
	}
	void arrayStart(const Token& arraY)
	{
		code += tabulacja() + "<array>\n";
		glebokosc++;
		code += tabulacja() + "<name>"+arraY.value+"</name>\n";
	}
	void arrayEnd()
	{
		glebokosc--;
		code += tabulacja() + "</array>\n";
	}
	void variabel(const Token& name)
	{
		code += tabulacja() + "<variable>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</variable>\n";
	}
	void subroutineCallStart()
	{
		code += tabulacja() + "<subrutineCall>\n";
		glebokosc++;
	}
	void subroutineCallEnd()
	{
		glebokosc--;
		code += tabulacja() + "</subrutineCall>\n";
	}
	void soubroutineName(const Token& name)
	{
		code +=tabulacja() + "<subrutineName>" + name.value + "<subrutineName>\n";
	}
	void soubroutineClassName(const Token& className, const Token& subroutineName)
	{
		code += tabulacja() + "<className>"+className.value + "</className>\n"
			+ tabulacja() + "<subrutineName>" + subroutineName.value + "<subrutineName>\n";
	}
	void expressionListStart()
	{
		code += tabulacja() + "<arguments>\n";
		glebokosc++;
	}
	void expressionListEnd()
	{
		glebokosc--;
		code += tabulacja() + "</arguments>\n";
	}
	void subrutineVariableDefinition(const Token& type, const Token& name)
	{
		code += tabulacja() + "<variableDefinition>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</variableDefinition>\n";
	}

private:
	NameMenager::NameMenager& nameMenager;
	string code;
	ofstream plikWyjsciowy;
	std::vector<std::string> tokenyNazwa{ "clasS", "constructor", "function", "method", "field", "statiC", "var", "inT", "chaR", "boolean", "voiD", "truE", "falsE", "nulL", "thiS", "leT", "dO", "iF", "elsE", "whilE", "returN", "curlyL", "curlyR", "roundL", "roundR", "squareL", "squareR", "dot", "comma", "semicolon", "plus", "minus", "star", "slash", "ampersand", "line", "angleL", "angleR", "equal", "tylda", "integerConstant", "stringConstant", "identifier" };
	int glebokosc = 0;//zmienna okreslajaca aktulana glebokosc w drzewie wyrazen 

	//funkcja zwraca nazwe typu (typ moze byc wbudowany, lub zdefiniowany przez uzytkownika)
	const string& typeName(const Token& t)
	{
		return (t.token != identifier ? tokenyNazwa[t.token] : t.value);
	}
	inline void otwarciePliku(string scierzkaWyjscia)
	{
		plikWyjsciowy.open(scierzkaWyjscia);
		if (!plikWyjsciowy.is_open())
		{
			throw std::exception(("nie mozna otworzyc pliku : " + scierzkaWyjscia).c_str());
		}
	}
	string tabulacja()
	{
		string t{};
		for (int i = 0; i < glebokosc; i++)
		{
			t += "\t";
		}
		return t;
	}

};

