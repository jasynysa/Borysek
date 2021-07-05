#pragma once
#include <string>
enum EnumToken
{
	clasS, constructor, function, method, field, statiC, var, inT, chaR, boolean, voiD, truE, falsE, nulL, thiS, leT, dO, iF, elsE, whilE, returN, curlyL, curlyR, roundL, roundR, squareL, squareR, dot, comma, semicolon, plus, minus, star, slash, ampersand, line, angleL, angleR, equal, tylda, integerConstant, stringConstant, identifier
};
struct Token
{
	Token(EnumToken token, std::string wartosc) :
		token(token),
		value(wartosc) {}
	EnumToken token;
	std::string value;
};