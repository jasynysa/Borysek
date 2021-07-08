#pragma once
#include<string>
#include"Token.h"
#include<fstream>
#include<vector>
#include<map>
using namespace std;

class Tokenizer;
typedef void(Tokenizer::* FPointer)();
class Tokenizer
{
public:
	Tokenizer(string adresZrodlo)
	{		
		try
		{
			otwarciePliku(adresZrodlo);
			wypelnianieMapy();
			ekstrakcjaTagow();
		}
		catch (const std::exception& e)
		{
			e.what();
		}
		
	}
	bool hasTokens(EnumToken t)
	{
		if (itVectorTokenow < vectorTokenow.size())
		{
			if (vectorTokenow[itVectorTokenow].token == t)
			{
				itVectorTokenow++;
				return true;
			}
		}
		return false;
	}
	//@szukaneTokeny- ciag szukanych tokenow
		//jezlei w aktualnej pozycji znajduja sie kolejno szukane tokeny to zwaraca true 
	bool hasTokens(vector<EnumToken> szukaneTokeny)
	{
		bool czyPasuje = false;
		if (itVectorTokenow + szukaneTokeny.size() - 1 < vectorTokenow.size())
		{
			czyPasuje = true;
			for (int i = 0; i < szukaneTokeny.size(); i++)
			{
				if (vectorTokenow[itVectorTokenow + i].token != szukaneTokeny[i])
				{
					czyPasuje = false;
				}
			}
		}
		if (czyPasuje)
		{
			itVectorTokenow += szukaneTokeny.size();
		}
		return czyPasuje;
	}

	bool hasTokens(bool (*f)(EnumToken))
	{
		if (itVectorTokenow < vectorTokenow.size())
		{
			if (f(vectorTokenow[itVectorTokenow].token))
			{
				itVectorTokenow++;
				return true;
			}
		}
		return false;
	}
	//@vektorFunkcji- vektor z regolami weglog ktorych kolejne tokeny sa sprawdzane
	//funkcja zprawdza czy podana regula pasuje do aktualnego miejsca jezeli tak to przesowa wskaznik aktualnego miejsca i zrawca true
	bool hasTokens(vector<bool (*)(EnumToken)>vektorFunkcji)
	{
		bool czyPasuje = false;
		if (itVectorTokenow + vektorFunkcji.size() - 1 < vectorTokenow.size())
		{
			czyPasuje = true;
			for (int i = 0; i < vektorFunkcji.size(); i++)
			{
				if (!vektorFunkcji[i](vectorTokenow[itVectorTokenow + i].token))
				{
					czyPasuje = false;
				}
			}
		}
		if (czyPasuje)
		{
			itVectorTokenow += vektorFunkcji.size();
		}
		return czyPasuje;
	}
	
	const string& getValue(int ofset)
	{
		int it = itVectorTokenow + ofset;
		if (it < vectorTokenow.size() && it >= 0)
		{
			return vectorTokenow[it].value;
		}
		else
		{
			throw exception("przesuniecie poza zakres");
		}
	}
	//return token pointed by (((actual token) pointer) moved by (given ofset))
	//@ofset- value witch modifi pointer to token to return 
	const Token& getToken(int ofset)
	{
		int it = itVectorTokenow + ofset;
		if (it < vectorTokenow.size() && it >= 0)
		{
			return vectorTokenow[it];
		}
		else
		{
			throw exception("przesuniecie poza zakres");
		}
	}


private:
	std::ifstream plikHack;
	std::vector<Token> vectorTokenow;
	std::map<std::string, FPointer> mapaTokenowIFunkcji;
	int itVectorTokenow = 0;



	inline void otwarciePliku(string scierzkaPlikHack)
	{
		//otwarcie plikow wejscia i wyjscia
		plikHack.open(scierzkaPlikHack);
		if (!plikHack.is_open())
		{
			throw std::exception(("nie mozna otworzyc pliku : " + scierzkaPlikHack).c_str());
		}
	}

	inline void ekstrakcjaTagow()
	{
		//ekstrakcja tagow i zapisanie ich do listy
		//odczytywanie pliku			
		// get length of file:
		plikHack.seekg(0, plikHack.end);
		int length = plikHack.tellg();
		plikHack.seekg(0, plikHack.beg);
		char* kod = new char[length];
		plikHack.read(kod, length);

		std::string slowo;
		auto it = mapaTokenowIFunkcji.end();
		//wydobywanie slowa 		
		for (int i = 0; i < length; i++)
		{
			if ((kod[i] >= 'A' && kod[i] <= 'z') || kod[i] == '_')//slowo kluczowe lub identyfikator
			{
				//rozpoczynam odczyt slowa
				do
				{
					slowo += kod[i];
					i++;
				} while ((kod[i] >= '0' && kod[i] <= 9) || (kod[i] >= 'A' && kod[i] <= 'z') || kod[i] == '_');//dopoki kod[i] jests litera lub liczba lub znakiem '_'
				i--;

				auto it = mapaTokenowIFunkcji.find(slowo);
				if (it != mapaTokenowIFunkcji.end())//to jest slowo kluczowe
				{
					(this->*(it->second))();//wywolanie funkcji z mapy funkcji
				}
				else //to jest identyfikator
				{
					vectorTokenow.push_back(Token(identifier, slowo));
				}
				slowo = "";
			}
			else if ((kod[i] >= '0' && kod[i] <= '9'))//liczba
			{
				do
				{
					slowo += kod[i];
					i++;
				} while ((kod[i] >= '0' && kod[i] <= '9'));
				i--;
				//zapisanie stalej liczny
				vectorTokenow.push_back(Token(EnumToken::integerConstant, slowo));
				slowo = "";
			}
			else if (kod[i] == '"')//string
			{
				i++;
				while ((kod[i] >= ' ' && kod[i] <= '~') || kod[i] == 9 && kod[i] != '"')
				{
					slowo += kod[i];
					i++;
				}
				i--;
				vectorTokenow.push_back(Token(stringConstant, slowo));
				slowo = "";
			}
			else if (kod[i] == '/')//komentarz
			{
				if (kod[++i] == '/')//komentarz
				{
					do//przesowanie i do konca lini
					{
						i++;
					} while (kod[i] == 9 || (kod[i] >= ' ' && kod[i] <= '~'));
				}
				i--;
			}
			else if (!((it = mapaTokenowIFunkcji.find(std::string(1, kod[i]))) == mapaTokenowIFunkcji.end()))//symbol
			{
				(this->*(it->second))();//wywolanie funkcji zapisujacej znaleziony token
			}
			else if (kod[i] >= '!' && kod[i] <= '~')//nierozpoznany symbol
			{
				do
				{
					slowo += kod[i];
					i++;
				} while (kod[i] >= '!' && kod[i] <= '~');
				i--;
				throw(std::exception(("nie rozppoznano symbolu : " + slowo).c_str()));
				slowo = "";
			}
		}
	}
	//funkcje( obslugujace( (znaleziony) token)). Powinny byc jako lambda ale mie wiem jak zdefiniowac wskaznik na funkcje lambda przechwytujaca pole kalsy
	void tClass()
	{
		vectorTokenow.push_back(Token(clasS, ""));
	}
	void tConstructor()
	{
		Token nowy(constructor, "");
		vectorTokenow.push_back(nowy);
	}
	void tFunction()
	{
		vectorTokenow.push_back(Token(function, ""));
	}
	void tMethod()
	{
		vectorTokenow.push_back(Token(method, ""));
	}
	void tField()
	{
		vectorTokenow.push_back(Token(field, ""));
	}
	void tStatic()
	{
		vectorTokenow.push_back(Token(statiC, ""));
	}
	void tVar()
	{
		vectorTokenow.push_back(Token(var, ""));
	}
	void tInt()
	{
		vectorTokenow.push_back(Token(inT, ""));
	}
	void tChar()
	{
		vectorTokenow.push_back(Token(chaR, ""));
	}
	void tBoolean()
	{
		vectorTokenow.push_back(Token(boolean, ""));
	}
	void tVoid()
	{
		vectorTokenow.push_back(Token(voiD, ""));
	}
	void tTrue()
	{
		vectorTokenow.push_back(Token(truE, ""));
	}
	void tFalse()
	{
		vectorTokenow.push_back(Token(falsE, ""));
	}
	void tNull()
	{
		vectorTokenow.push_back(Token(nulL, ""));
	}
	void tThis()
	{
		vectorTokenow.push_back(Token(thiS, ""));
	}
	void tLet()
	{
		vectorTokenow.push_back(Token(leT, ""));
	}
	void tDo()
	{
		vectorTokenow.push_back(Token(dO, ""));
	}
	void tIf()
	{
		vectorTokenow.push_back(Token(iF, ""));
	}
	void tElse()
	{
		vectorTokenow.push_back(Token(elsE, ""));
	}
	void tWhile()
	{
		vectorTokenow.push_back(Token(whilE, ""));
	}
	void tReturn()
	{
		vectorTokenow.push_back(Token(returN, ""));
	}
	void tCurlyL()
	{
		vectorTokenow.push_back(Token(curlyL, ""));
	}
	void tCurlyR()
	{
		vectorTokenow.push_back(Token(curlyR, ""));
	}
	void tRoundL()
	{
		vectorTokenow.push_back(Token(roundL, ""));
	}
	void tRoundR()
	{
		vectorTokenow.push_back(Token(roundR, ""));
	}
	void tSquerL()
	{
		vectorTokenow.push_back(Token(squareL, ""));
	}
	void tSquerR()
	{
		vectorTokenow.push_back(Token(squareR, ""));
	}
	void tDot()
	{
		vectorTokenow.push_back(Token(dot, ""));
	}
	void tComma()
	{
		vectorTokenow.push_back(Token(comma, ""));
	}
	void tSemicolon()
	{
		vectorTokenow.push_back(Token(semicolon, ""));
	}
	void tPlus()
	{
		vectorTokenow.push_back(Token(EnumToken::plus, ""));
	}
	void tMinus()
	{
		vectorTokenow.push_back(Token(EnumToken::minus, ""));
	}
	void tStar()
	{
		vectorTokenow.push_back(Token(star, ""));
	}
	void tSlash()
	{
		vectorTokenow.push_back(Token(slash, ""));
	}
	void tAmpersand()
	{
		vectorTokenow.push_back(Token(ampersand, ""));
	}
	void tLine()
	{
		vectorTokenow.push_back(Token(line, ""));
	}
	void tAngleL()
	{
		vectorTokenow.push_back(Token(angleL, ""));
	}
	void tAngleR()
	{
		vectorTokenow.push_back(Token(angleR, ""));
	}
	void tEqual()
	{
		vectorTokenow.push_back(Token(EnumToken::equal, ""));
	}
	void tTylda()
	{
		vectorTokenow.push_back(Token(tylda, ""));
	}
	void tIntegerConstant()
	{
		vectorTokenow.push_back(Token(integerConstant, ""));
	}
	inline void wypelnianieMapy()
	{
		mapaTokenowIFunkcji.emplace("constructor", &Tokenizer::tConstructor);
		mapaTokenowIFunkcji.emplace("class", &Tokenizer::tClass);
		mapaTokenowIFunkcji.emplace("function", &Tokenizer::tFunction);
		mapaTokenowIFunkcji.emplace("method", &Tokenizer::tMethod);
		mapaTokenowIFunkcji.emplace("field", &Tokenizer::tField);
		mapaTokenowIFunkcji.emplace("static", &Tokenizer::tStatic);
		mapaTokenowIFunkcji.emplace("var", &Tokenizer::tVar);
		mapaTokenowIFunkcji.emplace("int", &Tokenizer::tInt);
		mapaTokenowIFunkcji.emplace("char", &Tokenizer::tChar);
		mapaTokenowIFunkcji.emplace("boolean", &Tokenizer::tBoolean);
		mapaTokenowIFunkcji.emplace("void", &Tokenizer::tVoid);
		mapaTokenowIFunkcji.emplace("true", &Tokenizer::tTrue);
		mapaTokenowIFunkcji.emplace("false", &Tokenizer::tFalse);
		mapaTokenowIFunkcji.emplace("null", &Tokenizer::tNull);
		mapaTokenowIFunkcji.emplace("this", &Tokenizer::tThis);
		mapaTokenowIFunkcji.emplace("let", &Tokenizer::tLet);
		mapaTokenowIFunkcji.emplace("do", &Tokenizer::tDo);
		mapaTokenowIFunkcji.emplace("if", &Tokenizer::tIf);
		mapaTokenowIFunkcji.emplace("else", &Tokenizer::tElse);
		mapaTokenowIFunkcji.emplace("while", &Tokenizer::tWhile);
		mapaTokenowIFunkcji.emplace("return", &Tokenizer::tReturn);
		mapaTokenowIFunkcji.emplace("{", &Tokenizer::tCurlyL);
		mapaTokenowIFunkcji.emplace("}", &Tokenizer::tCurlyR);
		mapaTokenowIFunkcji.emplace("(", &Tokenizer::tRoundL);
		mapaTokenowIFunkcji.emplace(")", &Tokenizer::tRoundR);
		mapaTokenowIFunkcji.emplace("[", &Tokenizer::tSquerL);
		mapaTokenowIFunkcji.emplace("]", &Tokenizer::tSquerR);
		mapaTokenowIFunkcji.emplace(".", &Tokenizer::tDot);
		mapaTokenowIFunkcji.emplace(",", &Tokenizer::tComma);
		mapaTokenowIFunkcji.emplace(";", &Tokenizer::tSemicolon);
		mapaTokenowIFunkcji.emplace("+", &Tokenizer::tPlus);
		mapaTokenowIFunkcji.emplace("-", &Tokenizer::tMinus);
		mapaTokenowIFunkcji.emplace("*", &Tokenizer::tStar);
		mapaTokenowIFunkcji.emplace("/", &Tokenizer::tSlash);
		mapaTokenowIFunkcji.emplace("&", &Tokenizer::tAmpersand);
		mapaTokenowIFunkcji.emplace("|", &Tokenizer::tLine);
		mapaTokenowIFunkcji.emplace("<", &Tokenizer::tAngleL);
		mapaTokenowIFunkcji.emplace(">", &Tokenizer::tAngleR);
		mapaTokenowIFunkcji.emplace("=", &Tokenizer::tEqual);
		mapaTokenowIFunkcji.emplace("~", &Tokenizer::tTylda);
	}
};

