#pragma once
class SyntaxTree
{
public:
	class Node;
	using NodePointer = Node*;
private:

	class Node
	{
	public:
		Node(Token token, NodePointer parent):
			parent(parent)
		{
			vektorTokenow.push_back(token);
		}
		Node(std::vector<Token> vektorTokenow, NodePointer Parent):
			parent(parent),
			vektorTokenow(vektorTokenow)
		{}

		NodePointer getChild(int index)
		{
			return vectorChildrens[index];
		}

		std::vector<Token> vektorTokenow;
		Node* parent;
		std::vector<Node*> vectorChildrens;
	};


public:
	SyntaxTree() :
		root(NULL)
	{}
	NodePointer addNode(const std::vector<Token>& token, NodePointer parent)
	{
		NodePointer child = new Node(token, parent);
		parent->vectorChildrens.push_back(child);
		return child;
	}
	NodePointer addNode(const std::vector<Token>& token) //ading root node
	{		
		root = new Node(token, NULL);
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

	void pushOver(std::vector<Token> vektorTokenow, NodePointer child)
	{
		NodePointer parent = child->parent;

		//deleting poiner form parent to child
		for (std::vector<Node*>::iterator it = parent->vectorChildrens.begin(); it != parent->vectorChildrens.end(); it++)
		{
			if (*it == child)
			{
				parent->vectorChildrens.erase(it);
				break;
			}
		}

		//adding new node to parent
		NodePointer newNode = new Node(vektorTokenow, parent);

		//conecting child to new node
		newNode->vectorChildrens.push_back(child);
		child->parent = newNode;
	}
private:
	NodePointer root;
};

