#pragma once
#include<string>
#include<fstream>
#include<iostream>
#include<list>
#include<vector>
#include<map>
#include<set>
#include<sstream>

#define DEBUG

using namespace std;

struct Segment
{
	string nazwa;
	int adres;
};

const struct Segmenty
{	//adresy segmentow w pamieci
	const Segment constant{ "constant", 100 };
	const Segment SP{ "SP", 256 };
	const Segment LCL{ "LCL", 400 };
	const Segment ARG{ "ARG", 600 };
	const Segment pointer{ "pointer",800 };
	const Segment tmp{ "temp",1000 };
	const Segment THIS{ "THIS",3000 };
	const Segment THAT{ "THAT",3010 };	
};

/*
struktura zawiera stringi odpowiadajace piwrwszym slowa instrukcji robie to w taki sposob aby uniknac bledu i ulatwic modyfikacje
*/
const struct PierwszeSlowo
{
	//modyfikacje stosu
	const string pop = "pop";
	const string push = "push";
	//operacje
	const string add = "add";
	const string sub = "sub";
	const string neg = "neg";
	const string eq = "eq";
	const string gt = "gt";
	const string lt = "lt";
	const string anD = "and";
	const string oR = "or";
	const string noT = "not";
	 //sterowanie kolejnoscia wykonywania
	const string label = "label";
	const string gotO = "goto";
	const string ifGoto = "if-goto";
	 //obsluga funkcji
	const string function = "function";
	const string call = "call";
	const string returN = "return";
};

class Vmashine;
typedef void(Vmashine::* FPointer)(vector<string>&);//wskaznik na funkcje 

class Vmashine
{
public:
	Vmashine()
	{
		try
		{
			//wypelnianie kontenerow
			wypelnianieKontenerow();

			//otwieranie pliku i tlumaczenie
			tlumaczenie();
		}
		catch (const std::exception& e)
		{
			cout << "blad: " << e.what();
		}
	}
private:
	ifstream plikZrodlo;
	ofstream plikZapis;
	string scierzkaZrodlo = "C:\\Users\\jasyn\\OneDrive\\Desktop\\MemoryAccess\\BasicTest\\BasicTest.vm";
	string scierzkaZapis = "C:\\Users\\jasyn\\OneDrive\\Desktop\\MemoryAccess\\BasicTest\\BasicTest.asm";

	list<int>listaInstrukcji;
	map<const string, FPointer> mapaPierwszegoSlowaIFunkcji;//zeby wskaznik funkcji mogl dzialac trzeba bylo dodac przestrzen nazw Vmashine
	set<string> setSegmentowPamieci;

	//adresy segmentow w pamieci
	const Segmenty segmenty;
	//string pierwszych slowo instrukcji
	const PierwszeSlowo pierwszeSlowo;

	void tlumaczenie()
	{
		//otwieranie pliku zrodlowego
		plikZrodlo.open(scierzkaZrodlo);
		if (!plikZrodlo.is_open())
		{
			throw exception("nie mozna otwozyc pliku zrodlowego");
		}

		//tworzenie lub nadpisywanie pliku docelowego
		plikZapis.open(scierzkaZapis);
		if (!plikZapis.is_open())
		{
			throw exception("nie udalo sie utworzyc pliku zapisu");
		}

		plikZapis<<inicjaliacjaLokalizacjiSegmentowPamieci();

		//odczytywanie linia po lini
		while (!plikZrodlo.eof())
		{
			char* linia;
			linia = new char[1000];
			plikZrodlo.getline(linia, 1000);

			//wyodrebnianie slow z lini i zapisywanie ich do vectora slow
			int i = 0;
			vector<string> vectorSlow;
			string slowo;
			while (linia[i] != 0)//niezmiennik petli: za kazdym razem i wskazuje na indeks lini
			{
				//odszukiwanie symbolu i umieszczanie poszczegolnych slow w liscie slow
				if (linia[i] >= '!' && linia[i] <= '~')
				{
					slowo += linia[i];
				}
				else if (slowo.size() > 0)
				{
					vectorSlow.push_back(slowo);
					slowo = "";
				}
				i++;
			}
			if (slowo.size() > 0)
			{
				vectorSlow.push_back(slowo);
				slowo = "";
			}

			//tlumaczenie slow, lub pomijanie komentarza, lub zwracanie bledu
			if (vectorSlow.size() > 0)
			{
				if (vectorSlow[0].size() >= 2)
				{
					if (vectorSlow[0][0] == '/' && vectorSlow[0][1] == '/')
					{
						continue;//pomijanie lini komentarz
					}
					else
					{
						//tlumaczenie komendy
						//sprawdzanie czy pierwsze slowo jest poprawne 
						auto itPierwszeSlowoIFunkcja = mapaPierwszegoSlowaIFunkcji.find(vectorSlow[0]);
						if (itPierwszeSlowoIFunkcja == mapaPierwszegoSlowaIFunkcji.end())//to znaczy nie znaleziono
						{
							throw exception(("nie rozpoznana komenda : " + vectorSlow[0]).c_str());
						}
						else
						{//znaleziono komende, uruchamianie odpowiedniej funkcji
							FPointer wskaznik;
							wskaznik = itPierwszeSlowoIFunkcja->second;
							(this->*wskaznik)(vectorSlow);
						}
					}
				}
				else
				{
					throw exception(("nie rozpoznany symbol: " + vectorSlow[0]).c_str());
				}
			}
		}

		//zamykanie plikow
		plikZapis.close();
		plikZrodlo.close();
	}
	void wypelnianieKontenerow()
	{
		//piwrwsze slowo instrukcji
		//modyfikacja stosu
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.push, &Vmashine::push);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.pop, &Vmashine::pop);
		//operacja na stosie
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.add, &Vmashine::add);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.sub, &Vmashine::sub);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.neg, &Vmashine::neg);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.eq, &Vmashine::eq);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.gt, &Vmashine::gt);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.lt, &Vmashine::lt);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.anD, &Vmashine::anD);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.oR, &Vmashine::oR);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.noT, &Vmashine::noT);
		//sterowanie kolejnoscia wykonywania operacji
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.label, &Vmashine::label);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.gotO, &Vmashine::gotO);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.ifGoto, &Vmashine::ifGoto);
		//obsluga funkcji
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.function, &Vmashine::function);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.call, &Vmashine::call);
		mapaPierwszegoSlowaIFunkcji.emplace(pierwszeSlowo.returN, &Vmashine::returN);

		//wypelnianie zbioru segmentow pamieci
		setSegmentowPamieci.insert(segmenty.ARG.nazwa);
		setSegmentowPamieci.insert(segmenty.constant.nazwa);
		setSegmentowPamieci.insert(segmenty.LCL.nazwa);
		setSegmentowPamieci.insert(segmenty.pointer.nazwa);
		setSegmentowPamieci.insert(segmenty.SP.nazwa);
		setSegmentowPamieci.insert(segmenty.THAT.nazwa);
		setSegmentowPamieci.insert(segmenty.THIS.nazwa);
		setSegmentowPamieci.insert(segmenty.tmp.nazwa);
	}

	/*
	funkcje wywolywane jezeli pierwsze slowo vektora slow jest takie jak nazwa funkcji
	*/
	//modyfikacja stosu
	void push(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala push" << endl;
#endif // DEBUG

		//sprawdzanie czy to wepchniecie wartosci z zewnatrz
		if (vectorSlow.size() > 1)
		{
			bool czyToLiczba = true;
			int liczba;
			try
			{
				liczba = stoi(vectorSlow[1]);
			}
			catch (const std::exception&)
			{
				czyToLiczba = false;
			}
			if (czyToLiczba)//czy to liczba
			{
				//wprowadzanie wartosci z zewnatrz
				plikZapis
					<< "//push z zewnatrz" << endl
					<< "@" + vectorSlow[1] << endl
					<< "D=A" << endl
					<< pushFromD();
			}
			//czy to wepchniecie wartosci z pamieci
			else if (vectorSlow.size() > 2)
			{
				auto  itSegmentPamieci = setSegmentowPamieci.find(vectorSlow[1]);
				if (itSegmentPamieci == setSegmentowPamieci.end())
				{//nie znaleziono takiego segmentu
					throw exception(bladInfo(vectorSlow).c_str());
				}
				else
				{
					czyToLiczba = true;
					//sprawdzanie czy trzecie slowo to liczba
					try
					{
						liczba = stoi(vectorSlow[2]);
					}
					catch (const std::exception&)
					{
						czyToLiczba = false;
					}
					if (czyToLiczba)
					{//tlumaczenie wepchniecia wartosci z pamieci

						plikZapis
							<< "//push z pamieci" << endl
							//najpierw wyznaczam adres w pamieci
							<< "@" + vectorSlow[2] << endl
							<< "D=A" << endl
							<< "@" + *itSegmentPamieci << endl
							<< "A=D+M" << endl
							//zapisuje zawartosc pamieci do d
							<< "D=M" << endl
							//wpycham zawartosc d na stos
							<< pushFromD();
					}
					else
					{
						throw exception(bladInfo(vectorSlow).c_str());
					}
				}
			}
			else

			{
				throw exception(bladInfo(vectorSlow).c_str());
			}
		}
	}
	void pop(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala pop" << endl;
#endif // DEBUG


		//czy wektor ma odpowiednia dlugosc
		if (vectorSlow.size() > 2)
		{
			auto  itSegmentPamieci = setSegmentowPamieci.find(vectorSlow[1]);
			if (itSegmentPamieci == setSegmentowPamieci.end())
			{//nie znaleziono takiego segmentu
				throw exception(bladInfo(vectorSlow).c_str());
			}
			else
			{
				bool czyToLiczba = true;
				//sprawdzanie czy trzecie slowo to liczba
				try
				{
					stoi(vectorSlow[2]);
				}
				catch (const std::exception&)
				{
					czyToLiczba = false;
				}
				if (czyToLiczba)
				{//tlumaczenie zdjecia wartosci ze stosu i umieszczenie jej w pamieci papmieci
					plikZapis
						<< "//pop" << endl
						//najpierw wyznaczam adres gdzie zapisze zdjeta wartosc i zapisanie go do zmiennej "adres"
						<< "@" + vectorSlow[2] << endl
						<< "D=A" << endl
						<< "@" + *itSegmentPamieci << endl
						<< "D=D+M" << endl
						<< "@adres" << endl
						<< "M=D" << endl
						//zdjecie wartosci ze stosu i zapisanie jej w d
						<< popToD()
						//zapisanie zawartosci d do wczesniej wyliczonego miejsca w pamieci
						<< "@adres" << endl
						<< "A=M" << endl
						<< "M=D" << endl;
				}
				else
				{
					throw exception(bladInfo(vectorSlow).c_str());
				}
			}
		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}
	}
	// operacje na stosie
	void add(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala dodawanie" << endl;
#endif // DEBUG
		plikZapis
			<< "//dodawanie" << endl
			<< popToD()
			<< "@" + segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=D+M" << endl;
	}
	void sub(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala odejmowanie" << endl;
#endif // DEBUG
		plikZapis
			<< "//odejmowanie" << endl
			<< popToD()
			<< "@" + segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=D-M" << endl;
	}
	void neg(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala negowanie bitowe" << endl;
#endif // DEBUG
		plikZapis
			<< "//negacja bitowa" << endl
			<< "@" + segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=!M" << endl;
	}
	void eq(vector<string>& vectorSlow)
	{
#ifdef DEBUG
		cout << "dziala negowanie bitowe" << endl;
#endif // DEBUG
		static int licznikOperacji = 0;//zmienna potrzezbna do niesamowicie dziwacznego sposobu wyznaczania rownosci logicznej
		plikZapis
			<< "//rownosc logiczna" << endl
			//wyznaczam roznice i zapisuje ja w d
			<< "@" + segmenty.SP.nazwa << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "A=A-1" << endl
			<< "D=D-M" << endl
			// wykonuje odpowiednia instrukcje skoku
			<< "@eq" << licznikOperacji << endl
			<< "D;JEQ" << endl
			//jezeli nie jest rowne
			<< "@"<<segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=0" << endl
			<< "@eqEnd" << licznikOperacji << endl
			<< "0;JMP" << endl
			//jezeli jest rowne 
			<< "(eq" << licznikOperacji << ")" << endl
			<< "@"<<segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=!D" << endl//w d jest 0
			//koniec instrukcji
			<< "(eqEnd" << licznikOperacji << ")" << endl;
		licznikOperacji++;
	}
	void gt(vector<string>& vectorSlow)
	{
		static int licznikOperacji = 0;//zmienna potrzezbna do niesamowicie dziwacznego sposobu wyznaczania rownosci logicznej
		plikZapis
			<< "//wiekszosc logiczna" << endl
			//wyznaczam roznice i zapisuje ja w d
			<< "@" + segmenty.SP.nazwa << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "A=A-1" << endl
			<< "D=M-D" << endl
			// wykonuje odpowiednia instrukcje skoku
			<< "@gt" << licznikOperacji << endl
			<< "D;JGT" << endl
			//jezeli nie jest wieksze
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=0" << endl
			<< "@gtEnd" << licznikOperacji << endl
			<< "0;JMP" << endl
			//jezeli jest wieksz
			<< "(gt" << licznikOperacji << ")" << endl
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "D=0" << endl
			<< "M=!D" << endl
			//koniec instrukcji
			<< "(gtEnd" << licznikOperacji << ")" << endl;
		licznikOperacji++;
	}
	void lt(vector<string>& vectorSlow)
	{
		static int licznikOperacji = 0;//zmienna potrzezbna do niesamowicie dziwacznego sposobu wyznaczania rownosci logicznej
		plikZapis
			<< "//mniejszosc logiczna" << endl
			//wyznaczam roznice i zapisuje ja w d
			<< "@" + segmenty.SP.nazwa << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "A=A-1" << endl
			<< "D=M-D" << endl
			// wykonuje odpowiednia instrukcje skoku
			<< "@lt" << licznikOperacji << endl
			<< "D;JLT" << endl
			//jezeli nie jest mnijesze
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=0" << endl
			<< "@ltEnd" << licznikOperacji << endl
			<< "0;JMP" << endl
			//jezeli jest mniejsze
			<< "(lt" << licznikOperacji << ")" << endl
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "D=0" << endl
			<< "M=!D" << endl
			//koniec instrukcji
			<< "(ltEnd" << licznikOperacji << ")" << endl;
		licznikOperacji++;
	}
	void anD(vector<string>& vectorSlow)//dziwan nazwa funkcji poniewaz "and" jest juz w skladni c++
	{
		plikZapis
			<< "//and" << endl
			<< popToD() << endl
			<< "@"<<segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=D&M" << endl;
	}
	void oR(vector<string>& vectorSlow)
	{
		plikZapis
			<< "//or" << endl
			<< popToD() << endl
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=D|M" << endl;
	}
	void noT(vector<string>& vectorSlow)
	{
		plikZapis
			<< "//not" << endl
			<< "@" << segmenty.SP.nazwa << endl
			<< "A=M-1" << endl
			<< "M=!M" << endl;
	}
	//sterowanie kolejnoscia wykonywania operacji
	void label(vector<string>& vectorSlow)
	{
		if (vectorSlow.size() > 1)
		{
			plikZapis
				<<"//etykieta"<<endl
				<< "(label" << vectorSlow[1] << ")" << endl;
		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}
	}
	void gotO(vector<string>& vectorSlow)
	{
		if (vectorSlow.size() > 1)
		{
			plikZapis
				<< "//skok bezwarunkowy" << endl
				<< "@label" << vectorSlow[1] << endl
				<< "0;JMP" << endl;
		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}
	}
	void ifGoto(vector<string>& vectorSlow)
	{
		if (vectorSlow.size() > 1)
		{
			plikZapis
				<< "//skok warunkowy" << endl
				<< popToD() << endl
				<< "@label" << vectorSlow[1] << endl
				<< "D;JLT" << endl;
		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}
	}
	//obsluga funkcji	
	/*
	argumenty:	vectorSlow[1] nazwa funkcji
				vectorSlow[2] ilosc argumentow
	*/
	void call(vector<string>& vectorSlow)
	{// argumenty
		/*
		stos:

				argument0
				.
				.
				argumentN
		SP->
		*/
		if (vectorSlow.size() > 2)
		{
			static int licznikWywolan = 0;
			int przesuniecieARG0;
			try
			{
				przesuniecieARG0 = 5 + stoi(vectorSlow[2]);
			}
			catch (const std::exception&)
			{
				throw exception(bladInfo(vectorSlow).c_str());
			} 
			plikZapis
				<< "//wywolanie funkcji" << endl
				//wprowadzenie na stos adresu powrotnego
				<< "@" << "call" << licznikWywolan << endl
				<< "D=A" << endl
				<<pushFromD()
				//wprowadzenie na stos wskaznikana segment lokal bierzacej funkcji
				<< "@"<<segmenty.LCL.nazwa << endl
				<< "D=M" << endl
				<<pushFromD()
				//wprowadzenie na stos wskaznika na segment arg bierzacej funkcji
				<< "@" << segmenty.ARG.nazwa << endl
				<< "D=M" << endl
				<< pushFromD()
				//wprowadzenie na stos wskaznika na this
				<< "@" << segmenty.THIS.nazwa << endl
				<< "D=M" << endl
				<< pushFromD()
				//wprowadzenie na stos wskaznika na that
				<< "@" << segmenty.THAT.nazwa << endl
				<< "D=M" << endl
				<< pushFromD()
				//ustawienie ARG pod adres argument0
				<<"@"<<segmenty.SP.nazwa<<endl
				<<"D=M"<<endl
				<< "@"<<przesuniecieARG0 << endl
				<< "D=D-A" << endl
				<< "@"<<segmenty.ARG.nazwa << endl
				<< "M=D" << endl
				//ustawienie LCL tam gdzie wskazuje SP
				<< "@"<<segmenty.SP.nazwa << endl
				<< "D=M" << endl
				<< "@"<<segmenty.LCL.nazwa << endl
				<< "M=D" << endl
				//instrukcja skoku do wyznaczonej funkcji
				<< "@"<<"function"<<vectorSlow[1] << endl
				<< "0;JMP" << endl
				//etykieta powrotna
				<< "(call" << licznikWywolan<<")" << endl;

		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}

	}

	
	/*
	argumenty:	vectroSlow[1]-nazwa funkcji 
				vectorSlow[2]-ilosc zmiennych lokalnych
	*/
	void function(vector<string>& vectorSlow)
	{
		/*
		stos:

		ARG->		argument0
					.
					.
					argumentN
					return adres
					LCL poprzedniej funkcji
					ARG pf
					THIS pf
					THAT pf
		SP-> LCL->	
		
		*/
		if (vectorSlow.size() > 2)
		{			
			try
			{
				stoi(vectorSlow[2]);
			}
			catch (const std::exception&)
			{
				throw exception(bladInfo(vectorSlow).c_str());
			}			
			plikZapis
				<< "//funkcja" << endl
				//ustawienie etykiety funkcji
				<< "(function" << vectorSlow[1] << ")" << endl
				//ustawienie LCL na aktualne wskazanie SP
				<< "@" << segmenty.SP.nazwa << endl
				<< "D=M" << endl
				<< "@" << segmenty.LCL.nazwa << endl
				<< "M=D" << endl
				//wepchniecie wszystkich zmiennych lokalnych zainicjowanych zerami 
				<< "@" << vectorSlow[2] << endl
				<< "D=A" << endl

				//petla while(D!=0)
				<< "(inicjalizacjaStart" << vectorSlow[1] << ")" << endl
				<< "@inicjalizacjaStop" << vectorSlow[1] << endl
				<< "D;JEQ" << endl
				//{
					<< "@" << segmenty.SP.nazwa << endl
					<< "A=M" << endl
					<< "M=0" << endl
					<< "@" << segmenty.SP.nazwa << endl
					<< "M=M+1" << endl //SP++
					<< "D=D-1" << endl //D--									
				//}
				<< "@inicjalizacjaStart" << vectorSlow[1] << endl
				<< "0;JMP" << endl//jump start
				<< "(inicjalizacjaStop" << vectorSlow[1] << ")" << endl;
		}
		else
		{
			throw exception(bladInfo(vectorSlow).c_str());
		}
	}

	void returN(vector<string>& vectorSlow)
	{
		/*
		stos:
		
		ARG->	argument0
				.
				.
				argumentN
				return adres
				LCL poprzedniej funkcji
				ARG pf
				THIS pf
				THAT pf
		LCL->	local0
				.
				.
				localN
		SP->
		*/

		
		plikZapis
			<<"//return"<<endl
			//przepisanie wyniku po adres wskazywanym przez arg (wynik jest pod sp-1)
			<< popToD()
			<< "@" << segmenty.ARG.nazwa << endl
			<< "A=M" << endl
			<< "M=D" << endl
			//zapisanie do SP ARG+1
			<< "@" << segmenty.ARG.nazwa << endl
			<< "D=M+1" << endl
			<< "@" << segmenty.SP.nazwa << endl
			<< "M=D" << endl
			//zapisanie LCL do zmiennej frame
			<< "@" << segmenty.LCL.nazwa << endl
			<< "D=M" << endl
			<< "@frame" << endl
			<< "M=D" << endl
			//frame--  THAT=*frame
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "@" << segmenty.THAT.nazwa << endl
			<< "M=D" << endl
			//frame--; This =*frame
			<< "@frame" << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "@" << segmenty.THIS.nazwa << endl
			<< "M=D" << endl
			//frame--; ARG =*frame
			<< "@frame" << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "@" << segmenty.ARG.nazwa << endl
			<< "M=D" << endl
			//frame--; lokal =*frame
			<< "@frame" << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl
			<< "@" << segmenty.LCL.nazwa << endl
			<< "M=D" << endl
			//zapisywanie adresu powrotu do A i skok
			//frame--; This =*frame
			<< "@frame" << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "A=M" << endl
			<< "0;JMP" << endl;
		/*
		stos:
		ARG->	argument0 poprzednia funkcja		
				.
				.
		LCL->	local0 pf
				.
				.
				wynik
		SP->	argument1				<-- od tad w dol wszystko jest juz nie wazne
				.
				argumentN
				return adres
				LCL poprzedniej funkcji
				ARG pf
				THIS pf
				THAT pf
				local0
				.
				.
				localN
		
		*/
	}

	//funkcje elementarne
	string popToD()
	{
		stringstream s;
		s
			<< "@" + segmenty.SP.nazwa << endl
			<< "M=M-1" << endl
			<< "A=M" << endl
			<< "D=M" << endl;
		return s.str();
	}

	string pushFromD()
	{
		stringstream s;
		s << "@" + segmenty.SP.nazwa << endl
			<< "A=M" << endl
			<< "M=D" << endl
			<< "@" + segmenty.SP.nazwa << endl
			<< "M=M+1" << endl;
		return s.str();
	}

	string inicjaliacjaLokalizacjiSegmentowPamieci()
	{
		stringstream s;
		list<Segment> listaSegmentow;
		listaSegmentow.push_back(segmenty.SP);
		listaSegmentow.push_back(segmenty.ARG);
		listaSegmentow.push_back(segmenty.constant);
		listaSegmentow.push_back(segmenty.LCL);
		listaSegmentow.push_back(segmenty.pointer);
		listaSegmentow.push_back(segmenty.THAT);
		listaSegmentow.push_back(segmenty.THIS);
		listaSegmentow.push_back(segmenty.tmp);
		for (auto const& segment : listaSegmentow)
		{
			s
				<< "//ustawienie poczatku segmentu: \"" + segment.nazwa + "\" na adres: " + to_string(segment.adres) << endl
				<< ("@" + to_string(segment.adres)) << endl
				<< ("D=A") << endl
				<< ("@" + segment.nazwa) << endl
				<< ("M=D") << endl;
		}
		return s.str();
	}

	string bladInfo(vector<string>& vectorSlow)
	{
		string info = "nierozpoznano komendy: ";
		for (auto const& slowo : vectorSlow)
		{
			info += slowo + " ";
		}
		return info;
	}
};
