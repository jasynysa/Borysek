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
	bool parseClassSubroutineDeclarationAndGenerateCode()
	{
		//glebokosc++;
		////poczatek deklaracji funkcji klasy
		//plikXML << tabulacja() << "<subroutinesDec>" << std::endl;

		//glebokosc++;
		//while ( //dopoki iterator wskazuje na token oznaczajacy poczatek definicji funkcji
		//	[this]
		//	{
		//		if (itVectorTokenow < vectorTokenow.size())
		//		{
		//			if (vectorTokenow[itVectorTokenow].token == constructor || vectorTokenow[itVectorTokenow].token == function || vectorTokenow[itVectorTokenow].token == method)
		//			{
		//				return true;
		//			}
		//		}
		//		return false;
		//	}())
		//{
		//	itVectorTokenow++;
		//	if (itVectorTokenow + 2 < vectorTokenow.size())
		//	{
		//		if ((vectorTokenow[itVectorTokenow].token == voiD || isType(vectorTokenow[itVectorTokenow])) && vectorTokenow[itVectorTokenow++].token == identifier && vectorTokenow[itVectorTokenow++].token == roundL)
		//		{
		//			itVectorTokenow++;
		//			parseParamiterList();
		//			if()
		//			if (vectorTokenow[itVectorTokenow].token==roundR)
		//			{

		//			}
		//		}
		//		else
		//		{
		//			throw exception();
		//		}
		//	}
		//	else
		//	{
		//		throw exception();
		//	}
		//	
		//}

		//	//koniec deklaracji funkcji kalsy
		//	plikXML << tabulacja() << "</subroutinesDec>" << std::endl;
		//	glebokosc--;
		return false;
	}

	void parseParamiterList()
	{

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