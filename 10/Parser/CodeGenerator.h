#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<iostream>
#include "Token.h"
using namespace std;
class CodeGenerator
{
public:
	CodeGenerator(string scierzkazWyjscia)
	{
			otwarciePliku(scierzkazWyjscia);
		
		
	}

	void classDefinitionStart(string identifier)
	{
		plikWyjsciowy <<"<class>" << std::endl
			<< "\t<keyword> class </keyword>" << std::endl
			<< "\t<indentifier> " + identifier + "</identifier>" << std::endl
			<< "\t<symbol> { </symbol>" << std::endl;
		glebokosc++;
	}
	void classDefinitionEnd()
	{
		glebokosc--;
		plikWyjsciowy << "\t <symbol> } </symbol>" << std::endl
			<< "<\class>";
	}
	void classVariableDefinition(const Token& type, string name)
	{
		plikWyjsciowy << tabulacja() + "<variableDefinition>" << endl
						<< tabulacja() + "\t<type>" + typeName(type) + "</type>"<<endl
						<< tabulacja() + "\t<name>" + name + "<name>" << endl
						<< tabulacja() + "</variableDefinition>" << endl;

	}
	void classSoubroutineDefinitionStart(Token returnType, Token name)
	{
		plikWyjsciowy << tabulacja() + "<soubroutineDefinition>\n";
		glebokosc++;
		plikWyjsciowy << tabulacja() + "<returnType>" + typeName(returnType) + "</returnType>\n"
			+ tabulacja() + "<name>" + name.value + "</name>\n";


	}
	void classSoubroutineDefinitionEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</soubroutine definition>\n";
	}
	void parameterListStart()
	{
		plikWyjsciowy << tabulacja() + "<parameterList>\n";
		glebokosc++;
	}
	void parameterDefinition(const Token& type, const Token& name)
	{
		plikWyjsciowy << tabulacja() + "<parameter>" << endl
			<< tabulacja() + "\t<type>" + typeName(type) + "</type>" << endl
			<< tabulacja() + "\t<name>" + name.value + "<name>" << endl
			<< tabulacja() + "</parameter>" << endl;			
	}
	void parameterListEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</paremetreList>\n";
	}
	void subroutineBodyStart()
	{
		plikWyjsciowy << tabulacja() + "<subroutineBody>\n";
		glebokosc++;
	}
	void subroutineBodyEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</subroutineBody>\n";
	}
	void letStatemantStart()
	{
		plikWyjsciowy << tabulacja() + "<letStatemant>\n";
		glebokosc++;
	}
	void letStatemantEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</letStatemant>\n";
	}
	void ifStatemantStart()
	{
		plikWyjsciowy << tabulacja() + "<ifStatemant>\n";
		glebokosc++;
	}
	void ifStatemantEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</ifStatemant>\n";
	}
	void whileStatemantStart()
	{
		plikWyjsciowy << tabulacja() + "<whielStatemant>\n";
		glebokosc++;
	}
	void whileStatemantEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</whileStatemant>\n";
	}
	void doStatemantStart()
	{
		plikWyjsciowy << tabulacja() + "<doStatemant>\n";
		glebokosc++;

	}
	void doStatemantEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</doStatemant>\n";
	}
	void returnStatemantStart()
	{
		plikWyjsciowy << tabulacja() + "<returnStatemant>\n";
		glebokosc++;
	}
	void returnStatemantEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</returnStatemant>\n";
	}
	void letVariableStart(const Token& var)
	{
		plikWyjsciowy << tabulacja() + "<letVariable>\n";
		glebokosc++;
		plikWyjsciowy << tabulacja() + "<name>" + var.value + "</name>\n";
	}
	void letVariableEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</letVariable>\n";
	}
	void letVariableIndexStart()
	{
		plikWyjsciowy << tabulacja() + "<index>\n";
		glebokosc++;
	}
	void letVariableIndexEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</index>\n";
	}
	void expretionStart()
	{
		plikWyjsciowy << tabulacja() + "<expretion>\n";
		glebokosc++;
	}
	void expretionEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</expretion>\n";
	}
	void operatoR(const Token& op)
	{
		plikWyjsciowy << tabulacja() + "<operator>" + tokenyNazwa[op.token] + "</operator>\n";
	}
	void unaryOperator(const Token& op)
	{
		plikWyjsciowy << tabulacja() + "<unaryOperator>" + tokenyNazwa[op.token] + "</unaryOperator>\n";
	}
	void constant(const Token& constant)
	{
		plikWyjsciowy << tabulacja() + "<"+tokenyNazwa[constant.token]+">\n"
			+ tabulacja() + "\t<value>" + constant.value + "</value>\n" 
			"</" + tokenyNazwa[constant.token] + ">\n";
	}
	void keywordConstant(const Token& keyword)
	{
		plikWyjsciowy << tabulacja() + "<keywordConstant>" + tokenyNazwa[keyword.token]+"</keywordConstant>\n";
	}
	void arrayStart(const Token& arraY )
	{

	}
	void arrayEnd()
	{

	}
	void variabel()
	{

	}
	void subroutineCallStart()
	{

	}
	void subroutineCallEnd()
	{

	}
	void soubroutineName(const Token& name)
	{

	}
	void soubroutineClassName(const Token& className, const Token& subroutiNename)
	{

	}
	void expressionListStart()
	{

	}
	void expressionListEnd()
	{

	}

private:
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

