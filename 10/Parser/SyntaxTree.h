#pragma once
#include "Token.h"
#include "vector"
using namespace std;
class SyntaxTree
{
public:
	class Node;
	using NodePointer = Node*;
private:

	class Node
	{
	public:
		Node(Token token, NodePointer parent, string name):
			parent(parent)
		{
			vektorTokens.push_back(token);
		}
		Node(std::vector<Token> vektorTokenow, NodePointer parent, string name):
			parent(parent),
			vektorTokens(vektorTokenow),
			name(name)
		{}

		NodePointer getChild(int index)
		{
			return vectorChildrens[index];
		}

		string name;
		std::vector<Token> vektorTokens;
		Node* parent;
		std::vector<Node*> vectorChildrens;
	};


public:
	SyntaxTree() :
		root(NULL)
	{}
	NodePointer addNode(const std::vector<Token>& token, NodePointer parent, string name)
	{
		NodePointer child = new Node(token, parent, name);
		parent->vectorChildrens.push_back(child);
		return child;
	}
	NodePointer addNode(const std::vector<Token>& token, string name) //ading root node
	{		
		root = new Node(token, NULL, name);
		return root;
	}
	void removeNode(NodePointer  node)
	{
		for (int i = 0; i < node->vectorChildrens.size(); i++)
		{
			removeNode(node->vectorChildrens[i]);
		}
		//removing pointer form parent's child vector
		for (std::vector<Node*>::iterator it = node->parent->vectorChildrens.begin(); it!= node->parent->vectorChildrens.end(); it++)
		{
			if (*it==node)
			{
				node->parent->vectorChildrens.erase(it);
				break;
			}
		}
		delete node;
	}
	NodePointer getRoot()
	{
		return root;
	}

	void pushOver(std::vector<Token> vektorTokenow, NodePointer child, string name)
	{
		NodePointer parent = child->parent;

		//deleting poiner form parent to child
		for (std::vector<NodePointer>::iterator it = parent->vectorChildrens.begin(); it != parent->vectorChildrens.end(); it++)
		{
			if (*it == child)
			{
				parent->vectorChildrens.erase(it);
				break;
			}
		}

		//adding new node to parent
		NodePointer newNode = addNode(vektorTokenow, parent, name);

		//conecting child to new node
		newNode->vectorChildrens.push_back(child);
		child->parent = newNode;
	}

	bool isItVarName(const Token& identifier)
	{
		if (identifier.token != EnumToken::identifier)
		{
			throw exception("expected identifier"+ identifier.nrLini);
		}

		vector<NodePointer>vectorVariables = root->vectorChildrens[0]->vectorChildrens;

		for (auto var : vectorVariables)
		{
			if (var->vektorTokens[2].value == identifier.value)
			{
				return true;
			}
		}
		return false;
	}

	bool isItSubroutinesName(const Token& identifier)
	{
		if (identifier.token != EnumToken::identifier)
		{
			throw exception("expected identifier");
		}

		vector<NodePointer>vectorSubroutinesName = root->vectorChildrens[1]->vectorChildrens;

		for (auto subroutines : vectorSubroutinesName)
		{
			if (subroutines->vektorTokens[1].value == identifier.value)
			{
				return true;
			}
		}
		return false;
	}

	bool isItSubroutinesVariableName(const Token& identifier, NodePointer subroutinePtr)
	{
		if (identifier.token != EnumToken::identifier)
		{
			throw exception("expected identifier");
		}


		vector<NodePointer>vectorSubroutinesParameterName = subroutinePtr->vectorChildrens[0]->vectorChildrens;


		for (auto subroutinesParameter : vectorSubroutinesParameterName)
		{
			if (subroutinesParameter->vektorTokens[0].value == identifier.value)
			{
				return true;
			}
		}
		//is it have local var list
		if (subroutinePtr->vectorChildrens.size() >= 2)
		{
			if (subroutinePtr->vectorChildrens[1]->vectorChildrens.size() >= 1)
			{
				vector<NodePointer>vectorSubroutinesLocalName = subroutinePtr->vectorChildrens[1]->vectorChildrens[0]->vectorChildrens;

				for (auto subroutinesLocal : vectorSubroutinesLocalName)
				{
					if (subroutinesLocal->vektorTokens[0].value == identifier.value)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	

private:
	NodePointer root;
};

