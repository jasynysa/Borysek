#pragma once
#include "Token.h"
#include<map>
#include <string>
using namespace std;
class NameMenager
{
public:
	NameMenager()
	{

	}

	//return true if token is identifier and hasn t been useed as var name
	bool isItVarNewName(Token t)
	{
		if (t.token == identifier)
		{
			if (mapVarNames.count(t.value)==0)
			{
				mapVarNames.emplace(t.value, varCount++);
				return true;
			}
		}
		return false;
	}

	//return true if token is identifier and hasn t been useed as subroutine name
	bool isItSubrutineNewName(Token t)
	{
		if (t.token == identifier)
		{
			if (mapSubroutinesNames.count(t.value) == 0)
			{
				mapVarNames.emplace(t.value, varCount++);
				return true;
			}
		}
		return false;
	}

	//return true if tokens are identifier and var name hasn t been useed as given soubroutine's var name
	bool isItSubroutinesVarName(Token functionName, Token varName)
	{
		if (functionName.token == identifier && varName.token == identifier)
		{
			Subroutine s = mapSubroutinesNames.at(functionName.value);
			if (s.mapSubroutineVarsName.count(varName.value) == 0)
			{
				s.mapSubroutineVarsName.emplace(varName.value, s.varCount++);
				return true;
			}
		}
		return false;
	}

	//return var index depending on whether it is class variable or subroutine variable
	int getVarIndex(string subroutineName , string varName)
	{
		Subroutine s = mapSubroutinesNames.at(subroutineName);
		int index;
		try
		{
			return s.mapSubroutineVarsName.at(varName);			
		}
		catch (const std::exception & e)
		{
			return mapVarNames.at(varName);
		}
		throw exception(((string)"cant find variable named: '" + varName + "'").c_str());
	}
private:
	int varCount = 0;
	std::map<string, int> mapVarNames;	

	struct Subroutine
	{
		EnumToken type;
		map<string, int> mapSubroutineVarsName;
		int varCount = 0;
	};

	map<string, Subroutine> mapSubroutinesNames;

};

