#pragma once
#include<string>
#include<fstream>
#include<vector>
#include<iostream>
#include "Token.h"
#include "NameMenager.h"
using namespace std;
static class CodeGenerator
{
public:
	CodeGenerator(string scierzkazWyjscia, NameMenager::NameMenager& nm):
		nameMenager(nm)
	{
		otwarciePliku(scierzkazWyjscia);


	}

	struct codeGeneratorState
	{
		string::iterator it;
		int glebokosc;
	};
	//save actual state of code 
	codeGeneratorState save()
	{
		codeGeneratorState cgs;
		cgs.glebokosc = glebokosc;
		cgs.it = codeXML.end();
		return cgs;
	}
	//restore state of code from moment of call save function
	void restore(codeGeneratorState state)
	{
		codeXML.erase(state.it, codeXML.end());
		glebokosc = state.glebokosc;
	}
	void writeToFile()
	{
		plikWyjsciowyXML << codeXML;
	}

	void classDefinitionStart(string identifier)
	{
		codeXML += "<class>\n";
		 + "\t<keyword> class </keyword>\n";
		 + "\t<indentifier> " + identifier + "</identifier\n>";
		 + "\t<symbol> { </symbol\n>" ;
		glebokosc++;

		className = identifier;
	}
	void classDefinitionEnd()
	{
		glebokosc--;
		codeXML += "<\class>\n";
	}
	void classVariableDefinition(const Token& type, string name)
	{
		codeXML += tabulacja() + "<variableDefinition>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name + "<name>\n"
			+ tabulacja() + "</variableDefinition>\n";

	}
	void classSoubroutineDefinitionStart(Token returnType, Token name)
	{
		codeXML += tabulacja() + "<soubroutineDefinition>\n";
		glebokosc++;
		codeXML += tabulacja() + "<returnType>" + typeName(returnType) + "</returnType>\n"
			+ tabulacja() + "<name>" + name.value + "</name>\n";

		//codeVM+=
	}
	void classSoubroutineDefinitionEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</soubroutineDefinition>\n";
	}
	void parameterListStart()
	{
		codeXML += tabulacja() + "<parameterList>\n";
		glebokosc++;
	}
	void parameterDefinition(const Token& type, const Token& name)
	{
		codeXML += tabulacja() + "<parameter>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</parameter>\n";
	}
	void parameterListEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</paremetreList>\n";
	}
	void subroutineBodyStart()
	{
		codeXML += tabulacja() + "<subroutineBody>\n";
		glebokosc++;
	}
	void subroutineBodyEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</subroutineBody>\n";
	}
	void letStatemantStart()
	{
		codeXML += tabulacja() + "<letStatemant>\n";
		glebokosc++;
	}
	void letStatemantEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</letStatemant>\n";
	}
	void ifStatemantStart()
	{
		codeXML += tabulacja() + "<ifStatemant>\n";
		glebokosc++;
	}
	void ifStatemantEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</ifStatemant>\n";
	}
	void whileStatemantStart()
	{
		codeXML += tabulacja() + "<whielStatemant>\n";
		glebokosc++;
	}
	void whileStatemantEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</whileStatemant>\n";
	}
	void doStatemantStart()
	{
		codeXML += tabulacja() + "<doStatemant>\n";
		glebokosc++;

	}
	void doStatemantEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</doStatemant>\n";
	}
	void returnStatemantStart()
	{
		codeXML += tabulacja() + "<returnStatemant>\n";
		glebokosc++;
	}
	void returnStatemantEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</returnStatemant>\n";
	}
	void letVariableStart(const Token& var)
	{
		codeXML += tabulacja() + "<letVariable>\n";
		glebokosc++;
		codeXML += tabulacja() + "<name>" + var.value + "</name>\n";
	}
	void letVariableEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</letVariable>\n";
	}
	void letVariableIndexStart()
	{
		codeXML += tabulacja() + "<index>\n";
		glebokosc++;
	}
	void letVariableIndexEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</index>\n";
	}
	void expretionStart()
	{
		codeXML += tabulacja() + "<expretion>\n";
		glebokosc++;
	}
	void expretionEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</expretion>\n";
	}
	void operatoR(const Token& op)
	{
		codeXML += tabulacja() + "<operator>" + tokenyNazwa[op.token] + "</operator>\n";
	}
	void unaryOperator(const Token& op)
	{
		codeXML += tabulacja() + "<unaryOperator>" + tokenyNazwa[op.token] + "</unaryOperator>\n";
	}
	void constant(const Token& constant)
	{
		codeXML += tabulacja() + "<" + tokenyNazwa[constant.token] + ">\n"
			+ tabulacja() + "\t<value>" + constant.value + "</value>\n"
			+tabulacja()+"</" + tokenyNazwa[constant.token] + ">\n";
	}
	void keywordConstant(const Token& keyword)
	{
		codeXML += tabulacja() + "<keywordConstant>" + tokenyNazwa[keyword.token] + "</keywordConstant>\n";
	}
	void arrayStart(const Token& arraY)
	{
		codeXML += tabulacja() + "<array>\n";
		glebokosc++;
		codeXML += tabulacja() + "<name>"+arraY.value+"</name>\n";
	}
	void arrayEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</array>\n";
	}
	void variabel(const Token& name)
	{
		codeXML += tabulacja() + "<variable>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</variable>\n";
	}
	void subroutineCallStart()
	{
		codeXML += tabulacja() + "<subrutineCall>\n";
		glebokosc++;
	}
	void subroutineCallEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</subrutineCall>\n";
	}
	void soubroutineName(const Token& name)
	{
		codeXML +=tabulacja() + "<subrutineName>" + name.value + "<subrutineName>\n";
	}
	void soubroutineClassName(const Token& className, const Token& subroutineName)
	{
		codeXML += tabulacja() + "<className>"+className.value + "</className>\n"
			+ tabulacja() + "<subrutineName>" + subroutineName.value + "<subrutineName>\n";
	}
	void expressionListStart()
	{
		codeXML += tabulacja() + "<arguments>\n";
		glebokosc++;
	}
	void expressionListEnd()
	{
		glebokosc--;
		codeXML += tabulacja() + "</arguments>\n";
	}
	void subrutineVariableDefinition(const Token& type, const Token& name)
	{
		codeXML += tabulacja() + "<variableDefinition>\n"
			+ tabulacja() + "\t<type>" + typeName(type) + "</type>\n"
			+ tabulacja() + "\t<name>" + name.value + "<name>\n"
			+ tabulacja() + "</variableDefinition>\n";

		localVariablesQuantity++;
	}
	void subroutineVariableDeclarationEnd(const Token& functionName)
	{
		//in this point i know quantity of local variables and a can define function in vm language
		writeFunctionDefinition(functionName.value, localVariablesQuantity);
		localVariablesQuantity = 0;
	}

private:
	int localVariablesQuantity=0;
	string className;
	NameMenager::NameMenager& nameMenager;
	string codeXML;
	string codeVM;
	ofstream plikWyjsciowyXML;
	std::vector<std::string> tokenyNazwa{ "clasS", "constructor", "function", "method", "field", "statiC", "var", "inT", "chaR", "boolean", "voiD", "truE", "falsE", "nulL", "thiS", "leT", "dO", "iF", "elsE", "whilE", "returN", "curlyL", "curlyR", "roundL", "roundR", "squareL", "squareR", "dot", "comma", "semicolon", "plus", "minus", "star", "slash", "ampersand", "line", "angleL", "angleR", "equal", "tylda", "integerConstant", "stringConstant", "identifier" };
	int glebokosc = 0;//zmienna okreslajaca aktulana glebokosc w drzewie wyrazen 

	//funkcja zwraca nazwe typu (typ moze byc wbudowany, lub zdefiniowany przez uzytkownika)
	const string& typeName(const Token& t)
	{
		return (t.token != identifier ? tokenyNazwa[t.token] : t.value);
	}
	inline void otwarciePliku(string scierzkaWyjscia)
	{
		plikWyjsciowyXML.open(scierzkaWyjscia);
		if (!plikWyjsciowyXML.is_open())
		{
			throw std::exception(("nie mozna otworzyc pliku : " + scierzkaWyjscia).c_str());
		}
	}
	string tabulacja()
	{
		string t{};
		for (int i = 0; i < glebokosc; i++)
		{
			t += "\t";
		}
		return t;
	}

	using MemorySegment = string;
	using Arithmetic = string;

	void writePush(MemorySegment memorySegment, int index)
	{
		codeVM += "push " + memorySegment + " " + std::to_string(index)+"\n";
	}
	void writePop(MemorySegment memorySegment, int index)
	{
		codeVM += "pop " + memorySegment + " " + std::to_string(index) + "\n";
	}
	void writeAritmethic(Arithmetic arithmetic)
	{
		codeVM += arithmetic + "\n";
	}
	void writeLabel(string label)
	{
		codeVM += "label " + label + "\n";
	}
	void writeGoTo(string label)
	{
		codeVM += "goto " + label + "\n";
	}
	void writeIfGoTo(string label)
	{
		codeVM += "if-goto " + label + "\n";
	}
	void writeFunctionCall(string functionName, int argsQuantity)
	{
		codeVM += "call " + className + "." + functionName + " " + std::to_string(argsQuantity)+"\n";
	}
	void writeFunctionDefinition(string functionName, int localsQuantity)
	{
		codeVM += "function " + className + "." + functionName + " " + std::to_string(localsQuantity) + "\n";
	}
	void writeReturn()
	{
		codeVM += "return\n";
	}

};

