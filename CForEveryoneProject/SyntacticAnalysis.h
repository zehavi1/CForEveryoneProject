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
	shared_ptr<ASTNode> type(); // ניתוח סוגי משתנים

	//משתנים
	shared_ptr<ASTNode> declaration();
	shared_ptr<ASTNode> variable(Pattern typeVariable); // ניתוח משתנה
	shared_ptr<ASTNode> variable_list(Pattern typeVariable); // ניתוח מזהה

	//משפטי השמה והדפסה
	shared_ptr<ASTNode> print_statement();
	shared_ptr<ASTNode> variable_declaration(); // ניתוח הצהרות משתנים
	shared_ptr<ASTNode> expr_print();
	shared_ptr<ASTNode> assignment(); // ניתוח השמות
	shared_ptr<ASTNode> statement(); // ניתוח פקודות
	//פקודות if
	shared_ptr<ASTNode> if_else_statement(); // ניתוח פקודות if-else
	shared_ptr<ASTNode> elif_statement(); // ניתוח פקודות if-else-if
	shared_ptr<ASTNode> conditions(); // ניתוח תנאים
	shared_ptr<ASTNode> condition(); // ניתוח תנאי
	shared_ptr<ASTNode> comparison_operator(); // ניתוח אופרטורים להשוואה
	shared_ptr<ASTNode> logical_operator(); // ניתוח אופרטורים לוגיים
	shared_ptr<ASTNode> block(); // ניתוח בלוקים
	
	//לולאות
	shared_ptr<ASTNode> for_loop(); // ניתוח לולאות for
	shared_ptr<ASTNode> while_loop(); // ניתוח לולאות while
	shared_ptr<ASTNode> foreach_loop(); // ניתוח לולאות foreach
	shared_ptr<ASTNode> function_definition(); // ניתוח הגדרות פונקציות
	shared_ptr<ASTNode> collection(); // ניתוח אוספים
	shared_ptr<ASTNode> parameter_list(); // ניתוח רשימת פרמטרים
	
	shared_ptr<ASTNode> return_statement(); // ניתוח פקודת return
	vector<shared_ptr<ASTNode>> parse();
	//void printAST(const shared_ptr<ASTNode>& node, int depth = 0);
	void printASTNodes(const shared_ptr<ASTNode>& node);

	~SyntacticAnalysis();
};

