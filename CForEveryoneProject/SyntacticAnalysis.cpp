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
shared_ptr<ASTNode> SyntacticAnalysis::condition() {
	shared_ptr<ASTNode> left = expression(); // ניתוח הביטוי הראשון
	Token op = currentToken(); // טוקן אופרטור
	if (op.typeToken == TOK_EQUAL || op.typeToken == TOK_NOT_EQUAL ||
		op.typeToken == TOK_LESS || op.typeToken == TOK_GREATER ||
		op.typeToken == TOK_LESS_EQUAL || op.typeToken == TOK_GREATER_EQUAL) {
		nextToken(); // לעבור לטוקן הבא
		shared_ptr<ASTNode> right = expression(); // ניתוח הביטוי השני
		return make_shared<BinaryOpNode>(op, left, right); // החזרת צומת בינארית
	}
	throw runtime_error("Expected a comparison operator in condition");
}

shared_ptr<ASTNode> SyntacticAnalysis::range_condition() {
	shared_ptr<ParentNode> rangeNode = make_shared<ParentNode>("range");
	rangeNode->addChild( expression()); // ניתוח הביטוי הראשון
	Token op1 = currentToken(); // טוקן אופרטור ראשון
	if (op1.typeToken == TOK_LESS || op1.typeToken == TOK_GREATER) {
		rangeNode->addChild(match(op1.typeToken)); // להתאים את הטוקן הנוכחי לפונקציה
		rangeNode->addChild(expression()); // ניתוח הביטוי האמצעי
		Token op2 = currentToken(); // טוקן אופרטור שני
		if (op2.typeToken == TOK_LESS || op2.typeToken == TOK_GREATER) {
			rangeNode->addChild(match(op2.typeToken)); // להתאים את הטוקן הנוכחי לפונקציה
			rangeNode->addChild(expression()); // ניתוח הביטוי האחרון
			return rangeNode; // החזרת צומת טווח
		}
	}
	throw runtime_error("Expected a range comparison operator in range_condition");
}

// הוספת פונקציות לניתוח פקודות אם
shared_ptr<ASTNode> SyntacticAnalysis::if_statement() {
	shared_ptr<ParentNode> ifNode = make_shared<ParentNode>("if");
	ifNode->addChild(match(TOK_IF));
	ifNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after if"));
	ifNode->addChild(condition());
	ifNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	ifNode->addChild(block());

	return ifNode;
}

shared_ptr<ASTNode> SyntacticAnalysis::if_else_statement() {
	shared_ptr<ParentNode> ifElseNode = make_shared<ParentNode>("if_else");
	ifElseNode->addChild(match(TOK_IF));
	ifElseNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after if"));
	ifElseNode->addChild(condition());
	ifElseNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	ifElseNode->addChild(block());
	if (currentToken().typeToken == TOK_ELSE) {
		ifElseNode->addChild(match(TOK_ELSE));
		ifElseNode->addChild(block());
	}
	return ifElseNode;
}

// הוספת פונקציות לניתוח לולאות
shared_ptr<ASTNode> SyntacticAnalysis::for_loop() {
	shared_ptr<ParentNode> forNode = make_shared<ParentNode>("for");
	forNode->addChild(match(TOK_FOR));
	forNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after for"));
	forNode->addChild(assignment());
	forNode->addChild((match(TOK_SEMICOLON, "Expected ';' after assignment")));
	forNode->addChild(condition());
	forNode->addChild(match(TOK_SEMICOLON, "Expected ';' after condition"));
	forNode->addChild(assignment());
	forNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after for loop"));
	forNode->addChild(block());
	return forNode;
}

shared_ptr<ASTNode> SyntacticAnalysis::while_loop() {
	shared_ptr<ParentNode> whileNode = make_shared<ParentNode>("while");
	whileNode->addChild(match(TOK_WHILE));
	whileNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after while"));
	whileNode->addChild(condition());
	whileNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	whileNode->addChild(block());
	return whileNode;
}
//לא טוב בכלל צריך לשנות את הדקדוק של מערך
shared_ptr<ASTNode> SyntacticAnalysis::collection() {
	return match(TOK_ID, "Expected identifier in collection");
	

	//Token idToken = currentToken(); // טוקן עבור מזהה
	//if (idToken.typeToken == TOK_ID) {
	//	nextToken(); // לעבור לטוקן הבא
	//	if (currentToken().typeToken == TOK_LEFT_ARRAY) {
	//		nextToken(); // לעבור לטוקן הפותח של המערך
	//		std::vector<shared_ptr<ASTNode>> elements; // רשימה לאחסון ביטויים
	//		elements.push_back(expression()); // ניתוח הביטוי הראשון
	//		while (currentToken().typeToken == TOK_COMMA) {
	//			nextToken(); // לעבור לטוקן הבא
	//			elements.push_back(expression()); // ניתוח הביטוי הבא
	//		}
	//		Token rightArrayToken = currentToken(); // טוקן עבור סוגר המערך
	//		if (rightArrayToken.typeToken == TOK_RIGHT_ARRAY) {
	//			nextToken(); // לעבור לטוקן הבא
	//			//return make_shared<ArrayNode>(idToken, elements); // החזרת צומת מערך
	//		}
	//		throw runtime_error("Expected closing bracket for array");
	//	}
	//	return make_shared<IdentifierNode>(idToken); // החזרת מזהה
	//}
	//throw runtime_error("Expected an identifier in collection");
}

shared_ptr<ASTNode> SyntacticAnalysis::foreach_loop() {
	shared_ptr<ParentNode> foreachNode = make_shared<ParentNode>("foreach");
	match(TOK_FOREACH);
	match(TOK_OPEN_PAREN, "Expected '(' after foreach");
	match(TOK_ID, "Expected identifier after foreach");
	match(TOK_IN, "Expected 'in' after identifier");
	foreachNode->addChild(collection());
	match(TOK_CLOSE_PAREN, "Expected ')' after foreach");
	foreachNode->addChild(block());
	return foreachNode;
}

// הוספת פונקציה לניתוח הגדרות פונקציות
shared_ptr<ASTNode> SyntacticAnalysis::function_definition() {
	shared_ptr<ParentNode> funcNode = make_shared<ParentNode>("function");
	Pattern returnType = currentToken().typeToken;
	if (returnType == TOK_VOID) {
		funcNode->addChild(match(TOK_VOID));
	}
	else 
		funcNode->addChild(type());
	funcNode->addChild(match(TOK_ID, "Expected function name"));
	match(TOK_OPEN_PAREN, "Expected '(' after function name");
	funcNode->addChild(parameter_list());
	match(TOK_CLOSE_PAREN, "Expected ')' after parameter list");
	funcNode->addChild(block());
	return funcNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::type()
{
	Pattern p = currentToken().typeToken;
	if (p == TOK_DOUBLE_TYPE || p == TOK_INT_TYPE || p == TOK_CHAR_TYPE ||
 p == TOK_BOOL_TYPE || p == TOK_FLOAT_TYPE || p == TOK_STRING_TYPE || p == TOK_LONG_TYPE)
	{
		Token t = currentToken();
		nextToken();
		return make_shared<TokenNode>(t);
	}
	else
	{
		match(TOK_EOF, "Expected type");
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::parameter_list() {
	shared_ptr<ParentNode> paramListNode = make_shared<ParentNode>("parameters");
	if (currentToken().typeToken != TOK_CLOSE_PAREN) {
		paramListNode->addChild(type()); // סוג משתנה
		paramListNode->addChild(match(TOK_ID, "Expected identifier"));
		while (currentToken().typeToken == TOK_COMMA) {
			paramListNode->addChild(match(TOK_COMMA));
			paramListNode->addChild(type()); // סוג משתנה
			paramListNode->addChild(match(TOK_ID, "Expected identifier"));
		}
	}
	return paramListNode;
}
// הוספת פונקציה לניתוח בלוקים
shared_ptr<ASTNode> SyntacticAnalysis::block()
{
	shared_ptr<ParentNode> blockNode = make_shared<ParentNode>("block");
	match(TOK_OPEN_CURLY, "Expected '{' at the beginning of block");
	while (currentToken().typeToken != TOK_CLOSE_CURLY) {
		blockNode->addChild(statement());
	}
	match(TOK_CLOSE_CURLY, "Expected '}' at the end of block");
	return blockNode;
}

// הוספת פונקציה לניתוח הצהרות משתנים
shared_ptr<ASTNode> SyntacticAnalysis::variable_declaration() {
	shared_ptr<ParentNode> varDeclNode = make_shared<ParentNode>("variable_declaration");
	if (currentToken().typeToken == TOK_VAR)
		varDeclNode->addChild(match(TOK_VAR));
	else
		varDeclNode->addChild(type()); // סוג משתנה
	varDeclNode->addChild(match(TOK_ID, "Expected identifier"));
	match(TOK_ASSIGN, "Expected '=' after identifier");
	varDeclNode->addChild(expression());
	match(TOK_SEMICOLON, "Expected ';' at the end of declaration");
	return varDeclNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::return_statement()
{
	shared_ptr<ParentNode> returnNode = make_shared<ParentNode>("return");
	returnNode->addChild(match(TOK_RETURN));
	returnNode->addChild(expression());
	returnNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return returnNode;
}
// עדכון פונקציה statement
shared_ptr<ASTNode> SyntacticAnalysis::statement() {
	Pattern p = currentToken().typeToken;
	switch (p) {
	case TOK_IF:
		return if_statement();
	/*case TOK_IFRANGE:
		return if_range_statement();*/
	case TOK_FOR:
		return for_loop();
	case TOK_WHILE:
		return while_loop();
	case TOK_FOREACH:
		return foreach_loop();
	case TOK_PRINT:
		return print_statement();
	case TOK_RETURN:
		return return_statement();
	case TOK_VAR:
	case TOK_DOUBLE_TYPE:
	case TOK_INT_TYPE:
	case TOK_CHAR_TYPE:
	case TOK_BOOL_TYPE:
	case TOK_FLOAT_TYPE:
	case TOK_STRING_TYPE:
	case TOK_LONG_TYPE:
		return variable_declaration();
	case TOK_ID:
		return assignment();
	default:
		throw runtime_error("Unexpected token in statement");
	}
}

// ניתוח השמה-assignment--לא לשכוח לעשות גם לאתחול משתנים
shared_ptr<ASTNode> SyntacticAnalysis::assignment() {
	shared_ptr<ParentNode> startNode = make_shared<ParentNode>("assignment");
	startNode->addChild(match(TOK_ID, "Expected identifier"));
	Pattern p = currentToken().typeToken;
	switch (p)
	{
	case TOK_PLUS:
	case TOK_MINUS:
	case TOK_ASTERISK:
	case TOK_SLASH: {
		startNode->addChild(match(p));
	}
	case TOK_ASSIGN:
	{
		startNode->addChild(match(TOK_ASSIGN, "Expected '=' after identifier"));
		startNode->addChild(expression());
		break;
	}
	case TOK_INCREMENT:
	case TOK_DECREMENT:
	{
		startNode->addChild(match(p));
		if (currentToken().typeToken == TOK_ASSIGN)
		{
			startNode->addChild(match(TOK_ASSIGN, "Expected '=' after identifier"));
			startNode->addChild(expression());
		}
		else
		{
			startNode->addChild(expression());
	}
		break;
	default:
		break;
	}
	if (p == TOK_ASSIGN || p == TOK_INCREMENT || p == TOK_DECREMENT) {
		startNode->addChild(match(p));
	}
	else {
		throw runtime_error("Expected '=' after identifier");
	}
	
	startNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return startNode;
}
//shared_ptr<ASTNode> SyntacticAnalysis::statement() {
//	Pattern p = currentToken().typeToken;
//	switch (p)
//	{
//	case TOK_DOUBLE_TYPE:
//		break;
//	case TOK_INT_TYPE:
//		break;
//	case TOK_CHAR_TYPE:
//		break;
//	case TOK_BOOL_TYPE:
//		break;
//	case TOK_FLOAT_TYPE:
//		break;
//	case TOK_STRING_TYPE:
//		break;
//	case TOK_LONG_TYPE:
//		break;
//	case TOK_VOID:
//		break;
//	case TOK_INCLUDE:
//		break;
//	case TOK_VAR:
//		break;
//	case TOK_PRINT:
//		return print_statement();
//		break;
//	case TOK_ID:
//		return assignment();
//		break;
//	case TOK_INT:
//	{
//		Token t = currentToken();
//		nextToken();
//		return make_shared<TokenNode>(t);
//	}
//	default:
//		break;
//	}
//}
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

