#pragma once
#include "Token.h"
#include<map>
class NameMenager
{
public:
	NameMenager()
	{

	}

	bool isItVar(Token t)
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

	int getVarNumber(string varName)
	{

	}

	bool isItSubrutine(EnumToken t)
	{

	}

	bool isItSubroutinesVar(EnumToken functionName, EnumToken varname)
	{

	}

private:
	int varCount = 0;
	map<string, int> mapVarNames;	

	struct Subroutine
	{
		EnumToken type;
		map<string, int> mapSubroutineVarsName;
	};

	map<string, Subroutine> mapSubroutines;
};

