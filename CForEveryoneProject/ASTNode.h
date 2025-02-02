#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"
#include "Variable.h"
using namespace std;
// מחלקת בסיס לכל הצמתים
struct ASTNode {
	
   virtual void printASTNode(int depth=0) = 0;
    virtual ~ASTNode() {}
	
};
inline void printTabsDepth(int depth) {
	for (int i = 0; i < depth; ++i) {
		cout << "  ";
	}
}
struct TokenNode:ASTNode{
public:
	Token token;
	TokenNode(Token token) :token(token) {}
	TokenNode(Token &token) :token(token) {}
	void printASTNode(int depth=0) {
		printTabsDepth(depth);
		cout << "TokenNode: " << token.value << endl;
	}
};
struct ParentNode :ASTNode {
public:
	string name;
	vector<shared_ptr<ASTNode>> children;
	map<string, Variable> variableScope;
	ParentNode(const string& name, vector<shared_ptr<ASTNode>> children) :name(name), children(children) {}
	ParentNode(const string& name) :name(name){}
public:
	void printASTNode(int depth = 0) {
		printTabsDepth(depth);
		cout << "ParentNode: " << name << endl;
		for (auto child : children) {
			child->printASTNode(depth+1);
		}
	}
	void addChild(shared_ptr<ASTNode> child) {
		children.push_back(child);
	}
	void changeChild(shared_ptr<ASTNode> child, int index)
	{
		children[index] = child;
	}
};

// צומת עבור ביטויים בינאריים
struct BinaryOpNode : ASTNode {
public:
	string name = "BinaryOpNode";
	Token op;
	shared_ptr<ASTNode> left;
	shared_ptr<ASTNode> right;
	BinaryOpNode(const Token& op, shared_ptr<ASTNode> left, shared_ptr<ASTNode> right)
		: op(op), left(left), right(right) {
	}
	BinaryOpNode(string name, const Token& op, shared_ptr<ASTNode> left, shared_ptr<ASTNode> right)
		:name(name), op(op), left(left), right(right) {
	}
	void printASTNode(int depth = 0) {
		printTabsDepth(depth + 1);
		cout << name<<endl;
		printTabsDepth(depth + 1);
		cout << "Left: " << endl;
		left->printASTNode(depth + 1);
		printTabsDepth(depth);
		cout << "BinaryOpNode: " << op.value << endl;
		printTabsDepth(depth + 1);
		cout << "Right: " << endl;
		right->printASTNode(depth + 1);
	}
};
