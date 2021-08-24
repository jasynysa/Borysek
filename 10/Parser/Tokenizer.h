#pragma once
#include<string>
#include"Token.h"
#include<fstream>
#include<vector>
#include<map>
using namespace std;

class Tokenizer;
typedef void(Tokenizer::* FPointer)(int);
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
	bool isItHasTokens(EnumToken t)
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
	bool isItHasTokens(vector<EnumToken> szukaneTokeny)
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

	bool isItHasTokens(bool (*f)(EnumToken))
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
	bool isItHasTokens(vector<bool (*)(EnumToken)>vektorFunkcji)
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
	//int itVectorTokenowSave;


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
		int nrLini=1;
		//wydobywanie slowa 		
		for (int i = 0; i < length; i++)
		{
			if ((kod[i] >= 'A' && kod[i] <= 'Z') || (kod[i] >= 'a' && kod[i] <= 'z') || kod[i] == '_')//slowo kluczowe lub identyfikator
			{
				//rozpoczynam odczyt slowa
				do
				{
					slowo += kod[i];
					i++;
				} while ((kod[i] >= '0' && kod[i] <=  '9') || (kod[i] >= 'A' && kod[i] <= 'Z') || (kod[i] >= 'a' && kod[i] <= 'z') || kod[i] == '_');//dopoki kod[i] jests litera lub liczba lub znakiem '_'
				i--;

				auto it = mapaTokenowIFunkcji.find(slowo);
				if (it != mapaTokenowIFunkcji.end())//to jest slowo kluczowe
				{
					(this->*(it->second))(nrLini);//wywolanie funkcji z mapy funkcji
				}
				else //to jest identyfikator
				{
					vectorTokenow.push_back(Token(identifier, slowo, nrLini));
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
				vectorTokenow.push_back(Token(EnumToken::integerConstant, slowo, nrLini));
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
				vectorTokenow.push_back(Token(stringConstant, slowo,nrLini));
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
				(this->*(it->second))(nrLini);//wywolanie funkcji zapisujacej znaleziony token
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
			else if (kod[i] == '\n')//koniec lini
			{
				nrLini++;
				//cout << nrLini;
			}
		}
	}
	
	//funkcje( obslugujace( (znaleziony) token)). Powinny byc jako lambda ale mie wiem jak zdefiniowac wskaznik na funkcje lambda przechwytujaca pole kalsy
	void tClass(int nrLini)
	{
		vectorTokenow.push_back(Token(clasS, "", nrLini));
	}
	void tConstructor(int nrLini)
	{
		Token nowy(constructor, "", nrLini);
		vectorTokenow.push_back(nowy);
	}
	void tFunction(int nrLini)
	{
		vectorTokenow.push_back(Token(function, "", nrLini));
	}
	void tMethod(int nrLini)
	{
		vectorTokenow.push_back(Token(method, "", nrLini));
	}
	void tField(int nrLini)
	{
		vectorTokenow.push_back(Token(field, "", nrLini));
	}
	void tStatic(int nrLini)
	{
		vectorTokenow.push_back(Token(statiC, "", nrLini));
	}
	void tVar(int nrLini)
	{
		vectorTokenow.push_back(Token(var, "", nrLini));
	}
	void tInt(int nrLini)
	{
		vectorTokenow.push_back(Token(inT, "", nrLini));
	}
	void tChar(int nrLini)
	{
		vectorTokenow.push_back(Token(chaR, "", nrLini));
	}
	void tBoolean(int nrLini)
	{
		vectorTokenow.push_back(Token(boolean, "", nrLini));
	}
	void tVoid(int nrLini)
	{
		vectorTokenow.push_back(Token(voiD, "", nrLini));
	}
	void tTrue(int nrLini)
	{
		vectorTokenow.push_back(Token(truE, "", nrLini));
	}
	void tFalse(int nrLini)
	{
		vectorTokenow.push_back(Token(falsE, "", nrLini));
	}
	void tNull(int nrLini)
	{
		vectorTokenow.push_back(Token(nulL, "", nrLini));
	}
	void tThis(int nrLini)
	{
		vectorTokenow.push_back(Token(thiS, "", nrLini));
	}
	void tLet(int nrLini)
	{
		vectorTokenow.push_back(Token(leT, "", nrLini));
	}
	void tDo(int nrLini)
	{
		vectorTokenow.push_back(Token(dO, "", nrLini));
	}
	void tIf(int nrLini)
	{
		vectorTokenow.push_back(Token(iF, "", nrLini));
	}
	void tElse(int nrLini)
	{
		vectorTokenow.push_back(Token(elsE, "", nrLini));
	}
	void tWhile(int nrLini)
	{
		vectorTokenow.push_back(Token(whilE, "", nrLini));
	}
	void tReturn(int nrLini)
	{
		vectorTokenow.push_back(Token(returN, "", nrLini));
	}
	void tCurlyL(int nrLini)
	{
		vectorTokenow.push_back(Token(curlyL, "", nrLini));
	}
	void tCurlyR(int nrLini)
	{
		vectorTokenow.push_back(Token(curlyR, "", nrLini));
	}
	void tRoundL(int nrLini)
	{
		vectorTokenow.push_back(Token(roundL, "", nrLini));
	}
	void tRoundR(int nrLini)
	{
		vectorTokenow.push_back(Token(roundR, "", nrLini));
	}
	void tSquerL(int nrLini)
	{
		vectorTokenow.push_back(Token(squareL, "", nrLini));
	}
	void tSquerR(int nrLini)
	{
		vectorTokenow.push_back(Token(squareR, "", nrLini));
	}
	void tDot(int nrLini)
	{
		vectorTokenow.push_back(Token(dot, "", nrLini));
	}
	void tComma(int nrLini)
	{
		vectorTokenow.push_back(Token(comma, "", nrLini));
	}
	void tSemicolon(int nrLini)
	{
		vectorTokenow.push_back(Token(semicolon, "", nrLini));
	}
	void tPlus(int nrLini)
	{
		vectorTokenow.push_back(Token(EnumToken::plus, "", nrLini));
	}
	void tMinus(int nrLini)
	{
		vectorTokenow.push_back(Token(EnumToken::minus, "",nrLini));
	}
	void tStar(int nrLini)
	{
		vectorTokenow.push_back(Token(star, "", nrLini));
	}
	void tSlash(int nrLini)
	{
		vectorTokenow.push_back(Token(slash, "", nrLini));
	}
	void tAmpersand(int nrLini)
	{
		vectorTokenow.push_back(Token(ampersand, "", nrLini));
	}
	void tLine(int nrLini)
	{
		vectorTokenow.push_back(Token(line, "", nrLini));
	}
	void tAngleL(int nrLini)
	{
		vectorTokenow.push_back(Token(angleL, "", nrLini));
	}
	void tAngleR(int nrLini)
	{
		vectorTokenow.push_back(Token(angleR, "", nrLini));
	}
	void tEqual(int nrLini)
	{
		vectorTokenow.push_back(Token(EnumToken::equal, "", nrLini));
	}
	void tTylda(int nrLini)
	{
		vectorTokenow.push_back(Token(tylda, "", nrLini));
	}
	void tIntegerConstant(int nrLini)
	{
		vectorTokenow.push_back(Token(integerConstant, "", nrLini));
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

