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
			e.what();
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
			while (parseClassVariableDeclarationAndGenerateCode()) {}
			//subroutineDec*
			while (parseClassSubroutineDeclarationAndGenerateCode()) {}
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
	bool parseClassVariableDeclarationAndGenerateCode()
	{
		//('static' | 'field')
		if (!tokenizer.hasTokens({ [](EnumToken t) {return t == statiC || t == field ? true : false; } }))
		{
			return false;
		}
		//type varName
		else if (tokenizer.hasTokens({ isType,[](EnumToken t) {return t == identifier ? true : false; } }))
		{
			EnumToken typ= tokenizer.getToken(-2);
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
		//glebokosc++;		
		////poczatek deklaracji atrybotow
		//plikXML << tabulacja() << "<variablesDec>" << std::endl;

		//glebokosc++;
		//while ([this] {if (itVectorTokenow + 2 < vectorTokenow.size()) {
		//	if ((vectorTokenow[itVectorTokenow].token == statiC || vectorTokenow[itVectorTokenow].token == field) && isType(vectorTokenow[itVectorTokenow + 1]) && vectorTokenow[itVectorTokenow + 2].token == identifier) { return true; }
		//}return false; }())//czy poczatek deklaracji jeset prawidlowy
		//{//niezmiennik petli: itVector tokenow wskazuje na typ zmiennej
		//	EnumToken typ = vectorTokenow[itVectorTokenow + 1].token;

		//	do//niezmiennik petli: itVectorTokenow wskazuje na nazwe zmiennej
		//	{
		//		itVectorTokenow += 2;
		//		//zapisywanie atrybutu
		//		plikXML << tabulacja() + "<variable>" << endl
		//			<< tabulacja() + "\t<type>" + tokenyNazwa[typ] + "</type>"<<endl
		//			<< tabulacja() + "\t<name>" + vectorTokenow[itVectorTokenow].wartosc + "<name>" << endl
		//			<< tabulacja() + "</variable>" << endl;

		//	} while ([this] {if (itVectorTokenow + 2 < vectorTokenow.size()) {
		//		if (vectorTokenow[itVectorTokenow + 1].token == comma && vectorTokenow[itVectorTokenow + 2].token == identifier) { return true; }
		//	}return false; }());			
		//	itVectorTokenow++;

		//	//sprawdzanie czy na koncu jest srednik
		//	{
		//		bool czyJestSrednik = false;
		//		if (itVectorTokenow  < vectorTokenow.size())
		//		{
		//			if (vectorTokenow[itVectorTokenow].token == semicolon)
		//			{
		//				czyJestSrednik = true;
		//			}
		//		}
		//		if (!czyJestSrednik)
		//		{
		//			throw exception("brak srednika");
		//		}
		//		itVectorTokenow++;
		//	}
		//}
		//glebokosc--;

		//	//koniec deklaracji atrybutow
		//plikXML << tabulacja() << "</variablesDec>" << std::endl;
		//itVectorTokenow++;
		//glebokosc--;
	}

	//('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody
	bool parseClassSubroutineDeclarationAndGenerateCode()
	{
		//('constructor' | 'function' | 'method')
		if (!tokenizer.hasTokens({ [](EnumToken t) {return t==constructor||t==function||t==method? true: false; }}))
		{
			return false;
		}
		//('void' | type) subroutineName '(' 
		if (tokenizer.hasTokens({ [](EnumToken t) {return t == voiD || t == isType(t) ? true : false; },[](EnumToken t) {return t == identifier; },[](EnumToken t) {return t == roundL;} })
			
			&&
			//parameterList
			parseParamiterListAndGenerateCode()
			&&
			//')'
			tokenizer.hasTokens(roundR)
			&&
			//subroutineBody
			parseSubroutineBodyAndGenerateCode()		
			)
		{
			return true;
		}
		else
		{
			throw exception("wrong soubroutine definition");
		}
		return false;
	}

	bool parseParamiterListAndGenerateCode()
	{
		return false;
	}

	bool parseSubroutineBodyAndGenerateCode()
	{
		return false;
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