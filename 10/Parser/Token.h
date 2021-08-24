#pragma once
#include <string>
enum EnumToken
{
	clasS, constructor, function, method, field, statiC, var, inT, chaR, boolean,
	voiD, truE, falsE, nulL, thiS, leT, dO, iF, elsE, whilE, returN, curlyL, 
	curlyR, roundL, roundR, squareL, squareR, dot, comma, semicolon, plus, 
	minus, star, slash, ampersand, line, angleL, angleR, equal, tylda, 
	integerConstant, stringConstant, identifier, 
	local, parameter// local and parameter do not exist in code expicity, it is use leter by NameMenager class
};
struct Token
{
	Token(EnumToken token, std::string wartosc, int nrLini) :
		token(token),
		value(wartosc),
		nrLini(nrLini)
	{}
	EnumToken token;
	std::string value;
	int nrLini;
};