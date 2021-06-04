#pragma once
#include <string>
#include <fstream>
#include <map>
#include <list>
#include <iostream>
#include <bitset> //biblioteka umozliwia wyswietlanie liczby w formacie binarym
using namespace std;

class Assembler
{
public:
	Assembler()
	{
		//otwieranie pliku odczytu
		plikAsm.open(locationOpen);
		if (!plikAsm.is_open())
		{
			cout << "nie mozna otworzyc pliku";
			return;
		}

		//wypelnianie mapyInstrukcji
		wypelnianieMapyInstrukcji();

		//ekstrakcja symboli do listy stringow
		ekstrakcja();
	
		//obsluga etykiet
		etykiety();

		//obsluga zmiennych
		instrukcjeA();

		//tlumaczenie
		tlumaczenie();

		//zapisywanie do pliku
		zapisywanieDoPliku();

	}
private:
	string locationOpen = "C:\\Users\\jasyn\\OneDrive\\Desktop\\MemoryAccess\\BasicTest\\BasicTest.asm";
	string locationSave = "C:\\Users\\jasyn\\OneDrive\\Desktop\\MemoryAccess\\BasicTest\\BasicTest.hack";
	ifstream plikAsm;
	list<string> listaInstrukcji;
	list<int> listaKodowMaszynowych;
	map<string, int> mapaEtykiet;
	map<string, int> mapaInstrukcjiA;
	map<string, int> mapaInstrukcjiCel;
	map<string, int> mapaInstrukcjiObliczenia;
	map<string, int> mapaInstrukcjiSkok;

	///Uwaga kolejnosc wywolywania funkcji ma znaczenie dla prwidlowego tlumaczenia

	/**
	funkcja wycina wszystkie wyrazy, sprawdza czy to sa prawidlowe instrukcje i jezeli tak zapisuje je do listy instrukcji
	*/
	void ekstrakcja()
	{
		//odzczytywanie kolejnych lini az do konca pliku
		while (!plikAsm.eof())
		{
			//zapisywanie lini do tablicy charow
			char* linia;
			linia = new char[1000];
			plikAsm.getline(linia, 1000);

			//sprawdzanie kolejnych liter az do konca lini
			int i = 0;
			while (linia[i] != 0)//zero oznacza koniec stringu
			{
				//odzytywanie symbolu
				if (linia[i] >= '!' && linia[i] <= '~')
				{
					string symbol;
					do
					{
						symbol += linia[i];
						i++;
					} while ((linia[i] >= '!' && linia[i] <= '~') && linia[i] != 0);
					i--;

					//sprawdzanie czy to komentaz
					if (symbol.size() >= 2)
					{
						if (symbol[0] == '/' && symbol[1] == '/')
						{
							//to komentaz pomijam linie
							break;
						}
					}

					//sprawdzanie czy symbol jest prawidlowy
					bool czyPrawidlowy = false;

					//czy to jest instrukcja-C
					//
					if (tlumaczenieInstrukcjiC(symbol)!=-1)
					{
						czyPrawidlowy = true;
					}
					//czy to etykieta
					else if (symbol[0] == '(' && symbol[symbol.size() - 1] == ')')
					{
						bool czyEtykieta = false;
						for (int i = 1; i < symbol.size() - 2; i++)
						{
							if ((symbol[i] >= '!' && symbol[i] <= '~'))
							{
								czyEtykieta = true;
							}
							else
							{
								czyEtykieta = false;
								break;
							}
						}
						if (czyEtykieta)
						{
							czyPrawidlowy = true;
						}
					}
					//czy to instrukcja-a
					/*sprawdzam czy symbol ma na poczatku @ i pozniej litery albo cyfry*/
					else if (symbol[0] == '@')
					{
						bool czyOperacjaA = false;
						for (int i = 1; i < symbol.size(); i++)
						{
							if ((symbol[i] >= '!' && symbol[i] <= '~'))
							{
								czyOperacjaA = true;
							}
							else
							{
								czyOperacjaA = false;
								break;
							}
						}
						if (czyOperacjaA)
						{
							czyPrawidlowy = true;
						}
					}

					//wynik testu na prawidlowa instrukcje
					if (czyPrawidlowy)
					{
						//jezeli tak to dodaje symbol do listy inctrukcji 
						listaInstrukcji.push_back(symbol);
						//debug
						/*for (list<string>::iterator it = listaInstrukcji.begin(); it != listaInstrukcji.end(); it++)
						{
							cout << endl << *it;

						}*/
					}
					else
					{
						nierozpoznanySymbol(symbol);
						return;
					}
				}
				i++;
			}

		}
	}

	/**
	funkcja odnajduje etykiety i wi¹ze ich nazwy z adresami w ktrorych siê znajduj¹
	*/
	void etykiety()
	{
		int nrLini = 0;//wszystkie komendy oprocz etykiet zwiekszaja nr lini
		//przechodzenie przez wszystkie komendy
		string komendaDoUsuniecia;
		for (auto const& komenda : listaInstrukcji) //poruszanie sie po elementach listy od c++11
		{
			//usuwanie etykiety niezbyt wydajny sposob
			listaInstrukcji.remove(komendaDoUsuniecia);
			//poszukiwanie etykiety
			if (komenda[0] == '(')
			{
				//odzczytywanie nazwy etykiety
				string nazwaEtykiety;
				nazwaEtykiety.append(komenda, 1, komenda.size() - 2);

				//debug
				/*cout << endl << nazwaEtykiety;*/

				//dodawanie nazwy etykiety i nr lini gdzie ona wystepuje do mapy etykiet
				mapaEtykiet.insert(pair<string, int>(nazwaEtykiety, nrLini));
				komendaDoUsuniecia = komenda;
			}
			else
			{
				nrLini++;
			}
		}
	}

	/**
	funkcja zajmuje sie instrukcjami typu a:
	wyszukuje symbole oznaczajace zmienne , wiaze je w pary z odpowiednim kodem maszynowym i zapisuje do mapy instrukcjiA 
	*/
	void instrukcjeA()
	{
		//zmienna wskazujaca na wolny adres pamieciowy zaczyna sie od 16
		int wolnyAdres = 16;
		//przechodenie przez wszystie komendy
		for (auto const& komenda : listaInstrukcji)
		{
			//jezeli aktualna komenda to komenda-A
			if (komenda[0] == '@')
			{
				string instrukcjaA;
				instrukcjaA.append(komenda, 1, komenda.size() - 1);
				int wartosc;

				//jezeli jest to wprowadzanie wartosci
				if (komenda[1] >= 48 && komenda[1] <= 57)
				{
					wartosc = stoi(instrukcjaA);
				}
				//jezeli jest to zmienna lub etykieta
				else
				{
					//najpierw nalezy strawdzic czy zmiennej juz zostal przydzielony adres
					if (mapaInstrukcjiA.count(instrukcjaA))
					{
						//zmienna ma juz przypisany adres wiec przechodze do kolejnej komendy
						continue;
					}
					//czy to jest etykieta
					else if (mapaEtykiet.count(instrukcjaA))
					{
						wartosc = mapaEtykiet[instrukcjaA];
					}
					else
					{//brak takich zmiennych w mapach
						//przydzielenie zmiennej adresu i zwiekszenie wolnego adresu 
						wartosc = wolnyAdres++;
					}					
				}
				//debug
				/*cout << endl <<"zmienna:"<< instrukcjaA <<"wartosc"<< wartosc;*/

				//zapizanie poprzednio wyznaczonych zmiennych: 'zmienna' i 'wartosc' do mapyZmiennych				
				mapaInstrukcjiA.insert(pair<string,int>("@"+instrukcjaA, wartosc));
			}
		}
	}

	

	/**
	funkcja wypelnia listeKodow Maszynowych na podstawie listyInstrukcji i wczeœniej wype³nionych map
	*/
	void tlumaczenie()
	{
		cout<<endl << "---------------------";
		for (auto const& instrukcja : listaInstrukcji)
		{
			int kodMaszynowy;
			kodMaszynowy = tlumaczenieInstrukcjiC(instrukcja);
			if (kodMaszynowy != -1)//to znaczy ze aktualna instrukcja jest instrukcja C
			{
				listaKodowMaszynowych.push_back(kodMaszynowy);
			}
			else
			{//instrukcja jest typu A
				kodMaszynowy = mapaInstrukcjiA[instrukcja];
				listaKodowMaszynowych.push_back(kodMaszynowy);
			}
			//debug 
			bitset<16> kodMaszynowyBinarny(kodMaszynowy);
			
			cout << endl << instrukcja << "\t\t" << kodMaszynowyBinarny;
		}
	}

	void wypelnianieMapyInstrukcji()
	{
		//mapa instrukcji Obliczenia
		mapaInstrukcjiObliczenia["0"] =		0b0101010;
		mapaInstrukcjiObliczenia["1"] =		0b111111;
		mapaInstrukcjiObliczenia["-1"] =	0b111010;
		mapaInstrukcjiObliczenia["D"] =		0b001100;
		mapaInstrukcjiObliczenia["A"] =		0b110000;
		mapaInstrukcjiObliczenia["!D"] =	0b001101;
		mapaInstrukcjiObliczenia["!A"] =	0b110001;
		mapaInstrukcjiObliczenia["-D"] =	0b001111;
		mapaInstrukcjiObliczenia["-A"] =	0b110011;
		mapaInstrukcjiObliczenia["D+1"] =	0b011111;
		mapaInstrukcjiObliczenia["A+1"] =	0b110111;
		mapaInstrukcjiObliczenia["D-1"] =	0b001110;
		mapaInstrukcjiObliczenia["A-1"] =	0b110010;
		mapaInstrukcjiObliczenia["D+A"] =	0b000010;
		mapaInstrukcjiObliczenia["D-A"] =	0b010011;
		mapaInstrukcjiObliczenia["D|A"] =	0b010101;
		mapaInstrukcjiObliczenia["A-D"] =	0b000111;
		mapaInstrukcjiObliczenia["D&A"] =	0b000000;
		int bitA = 0b1000000;
		mapaInstrukcjiObliczenia["M"] = mapaInstrukcjiObliczenia["A"] + bitA;
		mapaInstrukcjiObliczenia["!M"] = mapaInstrukcjiObliczenia["!A"] + bitA;
		mapaInstrukcjiObliczenia["-M"] = mapaInstrukcjiObliczenia["-A"] + bitA;
		mapaInstrukcjiObliczenia["M+1"] = mapaInstrukcjiObliczenia["A+1"] + bitA;
		mapaInstrukcjiObliczenia["M-1"] = mapaInstrukcjiObliczenia["A-1"] + bitA;
		mapaInstrukcjiObliczenia["D+M"] = mapaInstrukcjiObliczenia["D+A"] + bitA;
		mapaInstrukcjiObliczenia["D-M"] = mapaInstrukcjiObliczenia["D-A"] + bitA;
		mapaInstrukcjiObliczenia["M-D"] = mapaInstrukcjiObliczenia["A-D"] + bitA;
		mapaInstrukcjiObliczenia["D&M"] = mapaInstrukcjiObliczenia["D&A"] + bitA;
		mapaInstrukcjiObliczenia["D|M"] = mapaInstrukcjiObliczenia["D|A"] + bitA;

		//mapa instrukcji miejsca docelowego zapisu
		mapaInstrukcjiCel[""] = 0;
		mapaInstrukcjiCel["M"] = 0b001;
		mapaInstrukcjiCel["D"] = 0b010;
		mapaInstrukcjiCel["MD"] = 0b011;
		mapaInstrukcjiCel["A"] = 0b100;
		mapaInstrukcjiCel["AM"] = 0b101;
		mapaInstrukcjiCel["AD"] = 0b110;
		mapaInstrukcjiCel["AMD"] = 0b111;

		//mapa instrukcji skoku
		mapaInstrukcjiSkok[""] = 0;
		mapaInstrukcjiSkok["JGT"] = 0b001;
		mapaInstrukcjiSkok["JEQ"] = 0b010;
		mapaInstrukcjiSkok["JGE"] = 0b011;
		mapaInstrukcjiSkok["JLT"] = 0b100;
		mapaInstrukcjiSkok["JNE"] = 0b101;
		mapaInstrukcjiSkok["JLE"] = 0b110;
		mapaInstrukcjiSkok["JMP"] = 0b111;

		//mapa predefiniowanych rejestrow
		mapaInstrukcjiA["@R0"] = 0;
		mapaInstrukcjiA["@R1"] = 1;
		mapaInstrukcjiA["@R2"] = 2;
		mapaInstrukcjiA["@R3"] = 3;
		mapaInstrukcjiA["@R4"] = 4;
		mapaInstrukcjiA["@R5"] = 5;
		mapaInstrukcjiA["@R6"] = 6;
		mapaInstrukcjiA["@R7"] = 7;
		mapaInstrukcjiA["@R8"] = 8;

	}

	/**
	funkcja tlumaczy innstrukcje c na kod maszynowy jezeli nie moze przetlumaczyc to zwraca -1
	*/
	int tlumaczenieInstrukcjiC(string symbol)
	{
		//odczytywanie dest comp i jump
		string dest, comp , jump;
		bool czyPojawiloSieR=false, czyPojawilSieSrednik = false;
		for (auto const& litera : symbol)
		{
			if (czyPojawilSieSrednik)//sprawdzanie czy litera jest srednikiem jest po sprawdzaniu czy pojawil sie srednik zeby do jump nie zapisac srednika
			{
				jump += litera;
			}

			if (litera == ';')//symbol oznacza ze konczy sie comp a zaczyna jump
			{
				czyPojawilSieSrednik = true;
			}
			
			if (czyPojawiloSieR && !czyPojawilSieSrednik)
			{
				comp += litera;
			}
			
			if (litera == '=')//sumbol '=' oznacza ze konczy sie dest a zaczyna komp
			{
				czyPojawiloSieR = true;
			}

			if (!czyPojawiloSieR && !czyPojawilSieSrednik)
			{
				dest += litera;
			}			
		}

		if (!czyPojawiloSieR)
		{
			comp = dest;
			dest = "";
		}
		
		//sprawdzanie czy instrukcja jest prawidlowa
		int kodMaszynowy;
		if (mapaInstrukcjiCel.count(dest)!=0 && mapaInstrukcjiObliczenia.count(comp)!=0 && mapaInstrukcjiSkok.count(jump)!=0)
		{//tlumaczenie instrukcji na kod maszynowy
			kodMaszynowy = mapaInstrukcjiSkok[jump];
			kodMaszynowy += mapaInstrukcjiCel[dest] * 0b1000;//przesuwanie bitow o 3 w gore
			kodMaszynowy += mapaInstrukcjiObliczenia[comp] * 0b1000000;//przesowanie o 6 bitow w gore
			kodMaszynowy += 0b1110000000000000; //inforamacja w kodzei maszynowym ze to instrukcaja typu C
		}
		else
		{
			kodMaszynowy = -1;
		}
		return kodMaszynowy;
		}

	void zapisywanieDoPliku()
	{
		//otwieranie pliku
		ofstream plikHack(locationSave);
		if (!plikHack.is_open())
		{
			cout << "!!!!!!!!nie udalo sie otworzyc pliku!!!!!!!!!!! " << locationSave;
			return;
		}
		//zapisywanie kodu maszynowego
		for (auto const& kodMaszynowy : listaKodowMaszynowych)
		{
			bitset<16> kodMaszynowyBinarny(kodMaszynowy);
			plikHack << kodMaszynowyBinarny << endl;
		}
		
		//zamykanie otwartych plikow
		plikAsm.close();
		plikHack.close();
		
	}
	
	void nierozpoznanySymbol(string symbol)
	{
		cout << "nierozpoznano symbolu :" + symbol;
	}
};

