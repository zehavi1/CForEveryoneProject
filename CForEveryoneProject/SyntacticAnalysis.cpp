#include "SyntacticAnalysis.h"
#include <iostream>
using namespace std;
SyntacticAnalysis::SyntacticAnalysis() {
}
SyntacticAnalysis::SyntacticAnalysis(vector<Token> tokens) : tokens(tokens) {
}
Token SyntacticAnalysis::currentToken() {
	if (currentTokenIndex >= tokens.size()) {
		throw runtime_error("Unexpected end of input");
	}
	return tokens[currentTokenIndex];
}
void SyntacticAnalysis::nextToken() {
	if (currentTokenIndex < tokens.size() - 1) {
		currentTokenIndex++;
	}
}
shared_ptr<TokenNode> SyntacticAnalysis::match(Pattern pattern, string msg) {
	if (currentToken().typeToken != pattern) {
		string s = "error occured in "+currentToken().value+'\n' + " describtion: " + msg;
		throw s;
	}
	Token t = currentToken();
	nextToken();
	return make_shared<TokenNode>(t); // ליצור ולהחזיר צומת חדש
}
shared_ptr<ASTNode> SyntacticAnalysis::expression() {
	shared_ptr<ASTNode> left = term();
	while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = term();
		left = make_shared<BinaryOpNode>(op, left, right);
	}
	return left;
}
shared_ptr<ASTNode> SyntacticAnalysis::term() {
	shared_ptr<ASTNode> left = factor();
	while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH || currentToken().typeToken == TOK_PERCENT) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = factor();
		left = make_shared<BinaryOpNode>(op, left, right);
	}
	return left;
}
shared_ptr<ASTNode> SyntacticAnalysis::factor() {
	shared_ptr<ParentNode> factorNode = make_shared<ParentNode>("factor");
	Pattern pattern = currentToken().typeToken;
	switch (pattern) {
	case TOK_ID:{
		shared_ptr<ASTNode> node=match(TOK_ID);
		return node;
	}
	case TOK_BOOL:
	case TOK_CHAR:
	case TOK_DOUBLE:
	case TOK_INT:
	case TOK_FLOAT:
	case TOK_LONG:
	{
		
		shared_ptr<ASTNode> node = number();
		
		return node;
	}
	case TOK_OPEN_PAREN:
	{
		factorNode->addChild(match(TOK_OPEN_PAREN));
		factorNode->addChild(expression());
		factorNode->addChild(match(TOK_CLOSE_PAREN, "Expected closing parenthesis"));
		return factorNode;
	}
	break;
	case TOK_INCREMENT:
	case TOK_DECREMENT:
	{
		factorNode->addChild(make_shared<TokenNode>(currentToken()));
		nextToken();
		factorNode->addChild(match(TOK_ID, "Expected identifier after increment/decrement"));
		return factorNode;
	}
	break;
	default:
		match(TOK_EOF, "Unexpected token in parseFactor");
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::number() {
	Pattern p = currentToken().typeToken;
	if (p == TOK_INT || p == TOK_DOUBLE || p == TOK_FLOAT || p == TOK_LONG || p == TOK_CHAR) {
		Token t = currentToken();
		nextToken();
		return make_shared<TokenNode>(t);
	}
	else {
		match(TOK_EOF, "Expected number");
	}
	return nullptr;
}
shared_ptr<ASTNode> SyntacticAnalysis::print_statement()
{
	shared_ptr<ParentNode> printNode = make_shared<ParentNode>("print");
	printNode->addChild(match(TOK_PRINT)); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after print")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(expression()); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after expression")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement")); // להתאים את הטוקן הנוכחי לפונקציה
	return printNode; // נניח שבינתיים משפט הוא ביטוי

}
// ניתוח השמה-assignment--לא לשכוח לעשות גם לאתחול משתנים
shared_ptr<ASTNode> SyntacticAnalysis::assignment() {
	shared_ptr<ParentNode> startNode = make_shared<ParentNode>("assignment");
	startNode->addChild(match(TOK_ID, "Expected identifier"));
	startNode->addChild(match(TOK_EQUAL, "Expected '=' after identifier"));
	startNode->addChild(expression());
	startNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return startNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::statement() {
	Pattern p = currentToken().typeToken;
	switch (p)
	{
	case TOK_DOUBLE_TYPE:
		break;
	case TOK_INT_TYPE:
		break;
	case TOK_CHAR_TYPE:
		break;
	case TOK_BOOL_TYPE:
		break;
	case TOK_FLOAT_TYPE:
		break;
	case TOK_STRING_TYPE:
		break;
	case TOK_LONG_TYPE:
		break;
	case TOK_VOID:
		break;
	case TOK_INCLUDE:
		break;
	case TOK_VAR:
		break;
	case TOK_PRINT:
		return print_statement();
		break;
	case TOK_ID:
		return assignment();
		break;
	case TOK_INT:
	{
		Token t = currentToken();
		nextToken();
		return make_shared<TokenNode>(t);
	}
	default:
		break;
	}
}
vector<shared_ptr<ASTNode>> SyntacticAnalysis::parse() {
	vector<shared_ptr<ASTNode>> ast;
	try {
		while (currentTokenIndex < tokens.size() - 1) {
			ast.push_back(statement());
		}
		return ast;
	}
	catch (string s) {
		cerr << s << endl;
	}
}
//void SyntacticAnalysis::printAST(const shared_ptr<ASTNode>& node, int depth) {
//	if (!node) return;
//
//	// הדפסת רווחים בהתאם לעומק העץ
//	for (int i = 0; i < depth; ++i) {
//		cout << "  ";
//	}
//
//	// הדפסת סוג הצומת
//	if (auto numNode = dynamic_pointer_cast<TokenNode>(node)) {
//		cout << "tokenNode: " << numNode->token.value << endl;
//	}
//
//	/*else if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node)) {
//		cout << "BinaryOpNode: " << binOpNode->op << endl;
//		for (int i = 0; i < depth; ++i) {
//			cout << "  ";
//		}*/
//		cout << "Left:" << endl;
//		printAST(binOpNode->left, depth + 1);  // צד שמאל
//		for (int i = 0; i < depth; ++i) {
//			cout << "  ";
//		}
//		cout << "Right:" << endl;
//		printAST(binOpNode->right, depth + 1); // צד ימין
//	}
//	else if (auto stmtNode = dynamic_pointer_cast<ParentNode>(node)) {
//		cout << "ParentNode of " << stmtNode->name << ":" << endl;
//		for (const auto& child : stmtNode->children)
//			printAST(child, depth + 1);
//	}
//	else {
//		cout << "Unknown Node" << endl;
//	}
//}
void SyntacticAnalysis::printASTNodes(const shared_ptr<ASTNode>& node)
{
	node->printASTNode();
}
SyntacticAnalysis::~SyntacticAnalysis() {
}

