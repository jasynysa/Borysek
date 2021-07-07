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
		plikWyjsciowy << "<class>" << std::endl
			<< "\t <keyword> class </keyword>" << std::endl
			<< "\t <indentifier> " + identifier + "</identifier>" << std::endl
			<< "\t <symbol> { </symbol>" << std::endl;
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
		plikWyjsciowy << tabulacja() + "<variable>" << endl
						<< tabulacja() + "\t<type>" + tokenyNazwa[type.token] + "</type>"<<endl
						<< tabulacja() + "\t<name>" + name + "<name>" << endl
						<< tabulacja() + "</variable>" << endl;

	}
	void classSoubroutineDefinitionStart(Token returnType, Token name)
	{
		plikWyjsciowy << tabulacja() + "<soubroutine definition>\n";
		glebokosc++;
		plikWyjsciowy << tabulacja() + "<return type>" + typeName(returnType) + "</return type>\n"
			+ tabulacja() + "<name>" + name.value + "</name>\n";


	}
	void classSoubroutineDefinitionEnd()
	{
		glebokosc--;
		plikWyjsciowy << tabulacja() + "</soubroutine definition>\n";
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

