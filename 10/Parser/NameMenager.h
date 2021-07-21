#pragma once
#include "Token.h"
#include<map>
#include <string>
using namespace std;
namespace NameMenager
{
	struct Var
	{
		int index;
		EnumToken kind;
	};

	class NameMenager
	{
	public:
		NameMenager()
		{

		}

		//return true if token is identifier and hasn t been useed as var name
		bool isItVarNewName(const Token& name, const Token& kind)
		{
			if (name.token == identifier)
			{
				throw exception("expected identiier");
			}
			if (kind.token != statiC && kind.token != field)
			{
				throw exception("wrong kind of variable");
			}
			if (mapVarNames.count(name.value) == 0)
			{
				Var var;
				if (kind.token == statiC)
				{
					var.index = varStaticCount++;
				}
				else
				{
					var.index = varFieldCount++;
				}
				var.kind = kind.token;
				mapVarNames.emplace(name.value, var);
				return true;
			}
			return false;
		}

		//return true if token is identifier and hasn t been useed as subroutine name
		bool isItSubrutineNewName(const Token& name, const Token& kind)
		{
			if (name.token != identifier)
			{
				throw exception("expectod identifier");
			}
			if (kind.token != method && kind.token != function && kind.token != constructor)
			{
				throw exception("wrong kind of subroutine");
			}
			if (mapSubroutinesNames.count(name.value) == 0)
			{
				Subroutine s;
				s.kind = kind.token;
				mapSubroutinesNames.emplace(name.value, s);
				return true;
			}
			return false;
		}

		//return true if tokens are identifier and var name hasn t been useed as given soubroutine's var name
		bool isItSubroutinesVarName(Token functionName, Token varName)
		{
			if (functionName.token != identifier || varName.token != identifier)
			{
				throw exception("expected identifer");
			}
			Subroutine s = mapSubroutinesNames.at(functionName.value);
			if (s.mapSubroutineVarsName.count(varName.value) == 0)
			{
				Var var;
				var.index = s.varCount++;
				var.kind = EnumToken::var;
				s.mapSubroutineVarsName.emplace(varName.value, var);
				return true;
			}
			return false;
		}

		//return var index depending on whether it is class variable or subroutine variable
		Var getVar(string subroutineName, string varName)
		{
			Subroutine s = mapSubroutinesNames.at(subroutineName);
			try
			{
				return s.mapSubroutineVarsName.at(varName);
			}
			catch (const std::exception& e)
			{
				return mapVarNames.at(varName);
			}
			throw exception(((string)"cant find variable named: '" + varName + "'").c_str());
		}
	private:
		int varFieldCount = 0;
		int varStaticCount = 0;

		std::map<string, Var> mapVarNames;


		struct Subroutine
		{
			EnumToken kind;
			map<string, Var> mapSubroutineVarsName;
			int varCount = 0;
		};

		map<string, Subroutine> mapSubroutinesNames;

	};

}