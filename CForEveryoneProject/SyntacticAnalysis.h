#pragma once
#include "Token.h"
#include <vector>
#include "ASTNode.h"
class SyntacticAnalysis
{
private:
	vector<Token> tokens; // רשימת הטוקנים
	size_t currentTokenIndex = 0;
public:
	SyntacticAnalysis();
	SyntacticAnalysis(vector<Token> tokens);
	Token currentToken();
	void nextToken();
	shared_ptr<TokenNode> match(Pattern pattern, string msg = "UnExcepted");
	shared_ptr<ASTNode> expression();
	shared_ptr<ASTNode> term();
	shared_ptr<ASTNode> factor();
	shared_ptr<ASTNode> number();
	shared_ptr<ASTNode> print_statement();
	shared_ptr<ASTNode> assignment();
	shared_ptr<ASTNode> parseStatement();
	vector<shared_ptr<ASTNode>> parse();
	void printAST(const shared_ptr<ASTNode>& node, int depth = 0);
	

	~SyntacticAnalysis();
};

