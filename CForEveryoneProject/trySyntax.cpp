//#include "Token.h"
//#include <vector>
//#include <fstream>
//#include <sstream>
//#include <iostream>
//class Syntax
//{
//
//public:
//	Syntax(vector<Token>& tokens):tokens(tokens){}
//	Tree& syntaxAnalisis()
//	{
//		//E->LIT|E OP E|not E
//		//LIT->TRUE|FALSE
//		//OP->and|or
//		
//	}
//	shared_ptr<Node> op()
//	{
//		Node node;
//		//string s = "op";
//		shared_ptr<Node> p();
//		
//		/*if (current == TOK_AND)
//			node.addChild(p);*/
//
//	}
//	shared_ptr<Node> match(Token t)
//	{
//		if (t.typeToken == current)
//		{
//			Node node;
//			shared_ptr<Node> p();
//			node.setToken(t);
//			nextToken();
//		}	
//		else
//			throw "syntax error in line " + t.lineNumber;
//	}
//	void nextToken()
//	{
//		current = tokens[++i].typeToken;
//	}
//
//private:
//	int i;
//	vector<Token> tokens;
//	Pattern current;
//};
//class Tree {
//private:
//	Node head;
//public:
//	Tree(Node& node)
//	{
//		head = node;
//	}
//};
//class Node {
//private:
//	string name;
//	Token token;
//	vector<shared_ptr<Node>> v;
//	int n;
//public:
//	Node() {};
//	Node(string name) :name(name){}
//	void setName(string name)
//	{
//		this->name = name;
//	}
//	void setToken(Token t)
//	{
//		this->token = t;
//	}
//	void addChild(shared_ptr<Node> n)
//	{
//		v.push_back(n);
//	}
//	void addChild(Node n)
//	{
//		shared_ptr<Node> p = make_shared<Node>(n);
//		v.push_back(p);
//	}
//};
//
//
