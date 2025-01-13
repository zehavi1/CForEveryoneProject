#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
using namespace std;
// מחלקת בסיס לכל הצמתים
struct ASTNode {
   virtual void printASTNode() = 0;
    virtual ~ASTNode() {}
};

// צומת עבור מזהים
//struct IdentifierNode : ASTNode {
//    string name;
//    IdentifierNode(const string& name) : name(name) {}
//    void printASTNode() {
//		cout << "IdentifierNode: " << name << endl;
//	}
//};
struct TokenNode:ASTNode{
	Token token;
	TokenNode(Token token) :token(token) {}
	void printASTNode() {
		cout << "TokenNode: " << token.typeToken << endl;
	}
};
struct ParentNode :ASTNode {
	string name;
	vector<shared_ptr<ASTNode>> children;
	ParentNode(const string& name, vector<shared_ptr<ASTNode>> children) :name(name), children(children) {}
	ParentNode(const string& name) :name(name){}
public:
	void printASTNode() {
		cout << "ParentNode: " << name << endl;
		for (auto child : children) {
			child->printASTNode();
		}
	}
	void addChild(shared_ptr<ASTNode> child) {
		children.push_back(child);
	}
	
};
// צומת עבור מספרים
struct NumberNode : ASTNode {
    double value;
    NumberNode(double value) : value(value) {}
    void printASTNode() {
		cout << "NumberNode: " << value << endl;
    }
};

// צומת עבור ביטויים בינאריים
struct BinaryOpNode : ASTNode {
    string op;
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;
    BinaryOpNode(const string& op, shared_ptr<ASTNode> left, shared_ptr<ASTNode> right)
        : op(op), left(left), right(right) {}
	void printASTNode() {
		cout << "BinaryOpNode: " << op << endl;
		left->printASTNode();
		right->printASTNode();
	}
};

// צומת עבור ביטויים
struct ExpressionNode : ASTNode {
    shared_ptr<ASTNode> expression;
    ExpressionNode(shared_ptr<ASTNode> expr) : expression(expr) {}
	void printASTNode() {
		cout << "ExpressionNode: " << endl;
		expression->printASTNode();
	}
};

// צומת עבור משפטים
struct StatementNode : ASTNode {
	shared_ptr<ASTNode> statement;
	StatementNode(shared_ptr<ASTNode> stmt) : statement(stmt) {}
	void printASTNode() {
		cout << "StatementNode: " << endl;
		statement->printASTNode();
	}
    
};

