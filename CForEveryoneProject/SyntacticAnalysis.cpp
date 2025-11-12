#include "SyntacticAnalysis.h"
#include <iostream>
#include "ErrorHandler.h"
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
		//string s = "Syntatic error occured in "+currentToken().value+'\n' + " describtion: " + msg;
		list<Token> v1;
		int i = currentTokenIndex-1;
		while (tokens[i].typeToken != TOK_SEMICOLON && tokens[i].typeToken != TOK_OPEN_CURLY)
			v1.push_front(tokens[i--]);
		string s = "Syntatic error occured after: ";
		for (Token i : v1)
		{
			s += i.value + " ";
		}
		s += "\nDescribtion: " + msg;
		throw ErrorHandler(s, currentToken().lineNumber);
	}
	Token t = currentToken();
	nextToken();
	return make_shared<TokenNode>(t); // ליצור ולהחזיר צומת חדש
}

// פונקציה לניתוח מספרים
shared_ptr<ASTNode> SyntacticAnalysis::number() {
	Pattern p = currentToken().typeToken;
	if (p == TOK_INT || p == TOK_DOUBLE || p == TOK_FLOAT || p == TOK_LONG || p == TOK_CHAR||p==TOK_BOOL) {
		Token t = currentToken();
		nextToken();
		return make_shared<TokenNode>(t, NUMBER);
	}
	else {
		match(TOK_EOF, "Expected number");
	}
	return nullptr;
}
//  פונקציה לניתוח בלוקים
shared_ptr<ASTNode> SyntacticAnalysis::block()
{
	shared_ptr<ParentNode> blockNode = make_shared<ParentNode>("block", BLOCK);
	if (currentToken().typeToken != TOK_OPEN_CURLY)
	{
		blockNode->addChild(statement());
	}
	else
	{
		blockNode->addChild(match(TOK_OPEN_CURLY, "Expected '{' at the beginning of block"));
		while (currentToken().typeToken != TOK_CLOSE_CURLY) {
			blockNode->addChild(statement());
		}
		blockNode->addChild(match(TOK_CLOSE_CURLY, "Expected '}' at the end of block"));
	}
	
	return blockNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::type()
{
	Pattern p = currentToken().typeToken;
	switch (p)
	{
	case TOK_DOUBLE_TYPE:
	case TOK_INT_TYPE:
	case TOK_CHAR_TYPE:
	case TOK_BOOL_TYPE:
	case TOK_FLOAT_TYPE:
	case TOK_STRING_TYPE:
	case TOK_LONG_TYPE:
	{
		return match(p);
		break;
	}

	default:
		match(TOK_EOF);
	}
}
// פונקציה לניתוח פקודת הדפסה
shared_ptr<ASTNode> SyntacticAnalysis::print_statement() {
	shared_ptr<ParentNode> printNode = make_shared<ParentNode>("print", PRINT_STATEMENT);
	printNode->addChild(match(TOK_PRINT)); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after print")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(expr_print()); // להתאים את הביטוי להדפסה
	printNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after expression")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement")); // להתאים את הטוקן הנוכחי לפונקציה
	return printNode; // החזרת צומת הדפסה
}
//<printf_statement> ::= TOK_SCANF TOK_OPEN_PAREN TOK_STRING_LITERAL TOK_STRING TOK_STRING_LITERAL (TOK_COMMA TOK_ID)* TOK_CLOSE_PAREN TOK_SEMICOLON
//write the function
shared_ptr<ASTNode> SyntacticAnalysis::printf_statement()
{
	shared_ptr<ParentNode> printfNode = make_shared<ParentNode>("printf", PRINTF_STATEMENT);
	printfNode->addChild(match(TOK_SCANF)); // להתאים את הטוקן הנוכחי לפונקציה
	printfNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after printf")); // להתאים את הטוקן הנוכחי לפונקציה
	printfNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal")); // להתאים את הטוקן הנוכחי לפונקציה
	printfNode->addChild(match(TOK_STRING, "Expected string")); // להתאים את הטוקן הנוכחי לפונקציה
	printfNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal")); // להתאים את הטוקן הנוכחי לפונקציה
	// ניתוח משתנים נוספים המופרדים בפסיקים
	while (currentToken().typeToken == TOK_COMMA) {
		printfNode->addChild(match(TOK_COMMA)); // לעבור על הפסיק
		printfNode->addChild(match(TOK_ID)); // ניתוח משתנה נוסף
	}
	printfNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after printf")); // להתאים את הטוקן הנוכחי לפונקציה
	printfNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement")); // להתאים את הטוקן הנוכחי לפונקציה
	return printfNode; // החזרת צומת הדפסה
}
// פונקציה לניתוח הביטויים להדפסה
shared_ptr<ASTNode> SyntacticAnalysis::expr_print() {
	shared_ptr<ParentNode> exprNode = make_shared<ParentNode>("expr_print",EXPR_PRINT);
	// ניתוח הביטוי הראשון או המחרוזת
	if (currentToken().typeToken == TOK_STRING_LITERAL) {
		exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
		exprNode->addChild(match(TOK_STRING, "Expected string"));
		exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
	}
	else {
		exprNode->addChild(expressionInPrint()); // ניתוח ביטוי
	}

	// חיבור עם '+' למחרוזות נוספות או ביטויים
	while (currentToken().typeToken == TOK_PLUS) {
		exprNode->addChild(match(TOK_PLUS)); // להתאים את סימן החיבור
		if (currentToken().typeToken == TOK_STRING_LITERAL) {
			exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
			exprNode->addChild(match(TOK_STRING, "Expected string"));
			exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
		}
		else {
			exprNode->addChild(expressionInPrint()); // ניתוח ביטוי נוסף
		}
	}

	return exprNode; // החזרת צומת הביטוי להדפסה
}
shared_ptr<ASTNode> SyntacticAnalysis::expressionInPrint() {
	shared_ptr<ASTNode> left = term();
	while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
		if (currentToken().typeToken == TOK_PLUS&&peekNextToken().typeToken == TOK_STRING_LITERAL)
		{
			return left;
		}
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = term();
		left = make_shared<BinaryOpNode>("expressionInPrint", op, left, right,EXPRESSION_IN_PRINT);
	}
	return left;
}
shared_ptr<ASTNode> SyntacticAnalysis::expression() {
	// כעת ביטוי מתחיל מהגרסה החדשה
	return exprOr();
}
// <expr_or> ::= <expr_and> (TOK_OR <expr_and>)*
shared_ptr<ASTNode> SyntacticAnalysis::exprOr() {
	shared_ptr<ASTNode> left = exprAnd();
	while (currentToken().typeToken == TOK_OR) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = exprAnd();
		left = make_shared<BinaryOpNode>(op, left, right,EXPR_OR);
	}
	return left;
}
// <expr_and> ::= <expr_comparison> (TOK_AND <expr_comparison>)*
shared_ptr<ASTNode> SyntacticAnalysis::exprAnd() {
	shared_ptr<ASTNode> left = exprComparison();
	while (currentToken().typeToken == TOK_AND) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = exprComparison();
		left = make_shared<BinaryOpNode>(op, left, right,EXPR_AND);
	}
	return left;
}
// <expr_comparison> ::= <expr_arithmetic> ((TOK_EQUAL | TOK_NOT_EQUAL | TOK_GREATER | TOK_LESS | TOK_GREATER_EQUAL | TOK_LESS_EQUAL) <expr_arithmetic>)*
shared_ptr<ASTNode> SyntacticAnalysis::exprComparison() {
	shared_ptr<ASTNode> left = exprArithmetic();
	Pattern p = currentToken().typeToken;
	p=currentToken().typeToken;
		while (p == TOK_EQUAL ||
			p == TOK_NOT_EQUAL ||
			p == TOK_GREATER ||
			p == TOK_LESS ||
			p == TOK_GREATER_EQUAL ||
			p == TOK_LESS_EQUAL) {
			Token op = currentToken();
			nextToken();
			shared_ptr<ASTNode> right = exprArithmetic();
			left = make_shared<BinaryOpNode>(op, left, right,EXPR_COMPARISON);
			// לבדוק אם יש טוקן נוסף מהסוג הנכון
			auto p1 = currentToken().typeToken;
			if (p1 == p) {
				op = currentToken();
				nextToken(); // לעבור לטוקן הבא
				shared_ptr<ASTNode> right = exprArithmetic();
				// אם יש טוקן נוסף באותו סוג, ניצור Node עבור תנאי הטווח
				left = make_shared<BinaryOpNode>("range", op, left, right,RANGE);
			}
			p = currentToken().typeToken;
		}
		return left;
}
// <expr_arithmetic> ::= <term> ((TOK_PLUS | TOK_MINUS) <term>)*
shared_ptr<ASTNode> SyntacticAnalysis::exprArithmetic() {
	shared_ptr<ASTNode> left = term();
	while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = term();
		left = make_shared<BinaryOpNode>(op, left, right,EXPR_ARITHMETIC);
	}
	return left;
}
shared_ptr<ASTNode> SyntacticAnalysis::term() {
	shared_ptr<ASTNode> left = factor();
	while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH || currentToken().typeToken == TOK_PERCENT) {
		Token op = currentToken();
		nextToken();
		shared_ptr<ASTNode> right = factor();
		left = make_shared<BinaryOpNode>(op, left, right,TERM);
	}
	return left;
}
shared_ptr<ASTNode> SyntacticAnalysis::factor() {
	shared_ptr<ParentNode> factorNode = make_shared<ParentNode>("factor",FACTOR);
	Pattern pattern = currentToken().typeToken;
	switch (pattern) {
	case TOK_ID: {
		shared_ptr<ASTNode> node = match(TOK_ID);
		if (currentToken().typeToken == TOK_LEFT_ARRAY)
		{
			factorNode->addChild(node);
			factorNode->addChild(match(TOK_LEFT_ARRAY));
			factorNode->addChild(expression());
			factorNode->addChild(match(TOK_RIGHT_ARRAY));
			return factorNode;
		}
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
//get next token
Token SyntacticAnalysis::peekNextToken() {
	if (currentTokenIndex + 1 >= tokens.size()) {
		throw runtime_error("Unexpected end of input while peeking next token");
	}
	return tokens[currentTokenIndex + 1];
}

shared_ptr<ASTNode> SyntacticAnalysis::comparison_operator() {
	Pattern p = currentToken().typeToken;
	switch (p)
	{
	case TOK_EQUAL:
	case TOK_NOT_EQUAL:
	case TOK_GREATER_EQUAL:
	case TOK_LESS_EQUAL:
	case TOK_LESS:
	case TOK_GREATER:
	{
		return match(p);
		break;
	}
	
	default:
		match(TOK_EOF, "Expected comparison operator");
		break;
	}
}

shared_ptr<ASTNode> SyntacticAnalysis::elif_statement() {
	shared_ptr<ParentNode> ifElseIfNode = make_shared<ParentNode>("elif",ELIF_STATEMENT);
	ifElseIfNode->addChild(match(TOK_ELIF));
	ifElseIfNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after elif"));
	ifElseIfNode->addChild(expression());
	ifElseIfNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	ifElseIfNode->addChild(block());
	return ifElseIfNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::if_else_statement() {
	shared_ptr<ParentNode> ifElseNode = make_shared<ParentNode>("if_else",IF_ELSE_STATEMENT);
	ifElseNode->addChild(match(TOK_IF));
	ifElseNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after if"));
	ifElseNode->addChild(expression());
	ifElseNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	ifElseNode->addChild(block());
	while (currentToken().typeToken == TOK_ELIF)
	{
		ifElseNode->addChild(elif_statement());
	}
	if (currentToken().typeToken == TOK_ELSE) {
		ifElseNode->addChild(match(TOK_ELSE));
		if (currentToken().typeToken == TOK_IF)
		{
			ifElseNode->addChild(if_else_statement());
		}
		else
		{
			ifElseNode->addChild(block());
		}
	}
	return ifElseNode;
}

// פונקציה לניתוח הצהרת משתנים
shared_ptr<ASTNode> SyntacticAnalysis::declaration()
{
    shared_ptr<ParentNode> declarationNode = dynamic_pointer_cast<ParentNode>(declaration1());
	if (declarationNode->name == "function")
		return declarationNode;
	declarationNode->addChild(match(TOK_SEMICOLON, "Expected ';' after variable declaration")); // לבדוק אם יש נקודה-פסיק
	return declarationNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::declaration1(bool canBeFunction) {
	shared_ptr<ParentNode> declarationNode = make_shared<ParentNode>("declaration",DECLARATION);
	Pattern typeVariable = currentToken().typeToken;
	Token typeVariableToken = currentToken();
	if (currentToken().typeToken == TOK_VAR)
	{
		declarationNode->addChild(match(TOK_VAR)); // לבדוק אם יש מילת מפתח var
	}
	else
	{
		declarationNode->addChild(type()); // ניתוח סוג המשתנה 
	}
	auto next = peekNextToken();
	if (canBeFunction && next.typeToken == TOK_OPEN_PAREN)
	{
		return function_definition(declarationNode);
	}
		
	else
	declarationNode->addChild(variable_list(typeVariable)); // ניתוח רשימת המשתנים
	//declarationNode->addChild(match(TOK_SEMICOLON, "Expected ';' after variable declaration")); // לבדוק אם יש נקודה-פסיק

	return declarationNode; // החזרת צומת הצהרת משתנים
}

// פונקציה לניתוח רשימת משתנים
shared_ptr<ASTNode> SyntacticAnalysis::variable_list(Pattern typeVariable) {
	shared_ptr<ParentNode> variableListNode = make_shared<ParentNode>("variable_list",VARIABLE_LIST);
	variableListNode->addChild(variable(typeVariable)); // ניתוח המשתנה הראשון

	// ניתוח משתנים נוספים המופרדים בפסיקים
	while (currentToken().typeToken == TOK_COMMA) {
		variableListNode->addChild(match(TOK_COMMA)); // לעבור על הפסיק
		variableListNode->addChild(variable(typeVariable)); // ניתוח משתנה נוסף
	}
	return variableListNode; // החזרת רשימת המשתנים
}
// פונקציה לניתוח משתנה
shared_ptr<ASTNode> SyntacticAnalysis::variable(Pattern typeVariable) {
	shared_ptr<ParentNode> variableNode = make_shared<ParentNode>("variable",VARIABLE);
	Token t = currentToken();
	if (t.typeToken == TOK_LEFT_ARRAY)
		return dynamic_array_declaration(typeVariable);
	if (t.typeToken == TOK_ASTERISK)
		return pointer_array_declaration();
	variableNode->addChild(match(TOK_ID)); // ניתוח מזהה המשתנה
	if (currentToken().typeToken == TOK_LEFT_ARRAY)
		return static_array_declaration(t);
	
	// בדיקה אם יש השמה
	if (currentToken().typeToken == TOK_ASSIGN|| typeVariable==TOK_VAR) {
		
		variableNode->addChild(match(TOK_ASSIGN,"var variable must be assigned")); // לעבור על סימן ההשמה
		if (typeVariable == TOK_STRING_TYPE)
		{
			variableNode->addChild(expr_print()); // ניתוח הביטוי המוקצה
		}
		else
		variableNode->addChild(expression()); // ניתוח הביטוי המוקצה
	}
	return variableNode; // החזרת צומת המשתנה
}
// הוספת פונקציה לניתוח הצהרות משתנים
shared_ptr<ASTNode> SyntacticAnalysis::variable_declaration() {
	bool isString = false;
	shared_ptr<ParentNode> varDeclNode = make_shared<ParentNode>("variable_declaration",VARIABLE_DECLARATION);
	if (currentToken().typeToken == TOK_VAR)
		varDeclNode->addChild(match(TOK_VAR));
	else
		if (currentToken().typeToken == TOK_STRING)
		{
			isString = true;
		}
		else
		varDeclNode->addChild(type()); // סוג משתנה
	varDeclNode->addChild(match(TOK_ID, "Expected identifier"));
	match(TOK_ASSIGN, "Expected '=' after identifier");
	varDeclNode->addChild(expression());
	match(TOK_SEMICOLON, "Expected ';' at the end of declaration");
	return varDeclNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::assignment() {
    shared_ptr<ParentNode> startNode = dynamic_pointer_cast<ParentNode>(assignment1());
    if (!startNode) {
        throw runtime_error("Failed to cast ASTNode to ParentNode");
    }
	startNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return startNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::assignment1() {
	shared_ptr<ParentNode> startNode = make_shared<ParentNode>("assignment",ASSIGNMENT);
	if (currentToken().typeToken != TOK_ID)
		return declaration1(false);
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
			
		}
		break;
	default:
		match(TOK_EOF, "error in assigment statement");
		break;
	}
	
	
	
	}
	return startNode;
}

//  פונקציות לניתוח לולאות
shared_ptr<ASTNode> SyntacticAnalysis::for_loop() {
	shared_ptr<ParentNode> forNode = make_shared<ParentNode>("for",FOR_LOOP);
	forNode->addChild(match(TOK_FOR));
	forNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after for"));
	forNode->addChild(assignment());
	forNode->addChild(expression());
	forNode->addChild(match(TOK_SEMICOLON, "Expected ';' after condition"));
	forNode->addChild(assignment1());
	forNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after for loop"));
	forNode->addChild(block());
	return forNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::while_loop() {
	shared_ptr<ParentNode> whileNode = make_shared<ParentNode>("while",WHILE_LOOP);
	whileNode->addChild(match(TOK_WHILE));
	whileNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after while"));
	whileNode->addChild(expression());
	whileNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	whileNode->addChild(block());
	return whileNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::collection() {
	Token idToken = currentToken(); // טוקן עבור מזהה
	if (idToken.typeToken == TOK_ID)
		return match(TOK_ID, "Expected identifier in collection");
	else
	{
		return full_array();
		//shared_ptr<ParentNode> collectionNode = make_shared<ParentNode>("collection",COLLECTION);
		//if (currentToken().typeToken == TOK_LEFT_ARRAY) {
		//	collectionNode->addChild(match(TOK_LEFT_ARRAY, "expected an array"));
		//	collectionNode->addChild(expression()); // ניתוח הביטוי הראשון
		//	while (currentToken().typeToken == TOK_COMMA) {
		//		nextToken(); // לעבור לטוקן הבא
		//		collectionNode->addChild(expression()); // ניתוח הביטוי הבא
		//	}
		//	collectionNode->addChild(match(TOK_RIGHT_ARRAY, "Expected closing bracket for array"));
		//		return collectionNode;
		//}
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::foreach_loop() {
	shared_ptr<ParentNode> foreachNode = make_shared<ParentNode>("foreach",FOREACH_LOOP);
	foreachNode->addChild( match(TOK_FOREACH));
	foreachNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after foreach"));
	try
	{
		foreachNode->addChild(type());
	}
	catch (string s)
	{

	}
	foreachNode->addChild(match(TOK_ID, "Expected identifier after foreach"));
	foreachNode->addChild(match(TOK_IN, "Expected 'in' after identifier"));
	foreachNode->addChild(collection());
	foreachNode->addChild(match(TOK_COMMA, "Expected , after collection"));
	if (currentToken().typeToken == TOK_ID)
	{
		foreachNode->addChild(match(TOK_ID, "Expected size after , in foreach"));
	}
	else
	{
		foreachNode->addChild(match(TOK_INT, "Expected size after , in foreach"));
	}
	foreachNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after foreach"));
	foreachNode->addChild(block());
	return foreachNode;
}
// הוספת פונקציה לניתוח הגדרות פונקציות
shared_ptr<ASTNode> SyntacticAnalysis::function_definition(shared_ptr<ParentNode> p) {
	shared_ptr<ParentNode> funcNode = make_shared<ParentNode>("function",FUNCTION_DEFINITION);
	Pattern returnType;
	if (p != nullptr)
	{
		returnType =dynamic_pointer_cast<TokenNode>(p->children[0])->token.typeToken;
		funcNode->addChild(p->children[0]);
	}
	else {
		returnType = currentToken().typeToken;
		if (returnType == TOK_VOID) {
			funcNode->addChild(match(TOK_VOID));
		}
		else
			funcNode->addChild(type());
		
	}
	
	funcNode->addChild(match(TOK_ID, "Expected function name"));
	funcNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after function name"));
	funcNode->addChild(parameter_list());
	funcNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after parameter list"));
	funcNode->addChild(block());
	return funcNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::parameter_list() {
	shared_ptr<ParentNode> paramListNode = make_shared<ParentNode>("parameters",PARAMETER_LIST);
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
shared_ptr<ASTNode> SyntacticAnalysis::return_statement()
{
	shared_ptr<ParentNode> returnNode = make_shared<ParentNode>("return",RETURN_STATEMENT);
	returnNode->addChild(match(TOK_RETURN));
	returnNode->addChild(expression());
	returnNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return returnNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::full_array() {
	shared_ptr<ParentNode> collectionNode = make_shared<ParentNode>("full_array",FULL_ARRAY);
	if (currentToken().typeToken == TOK_OPEN_CURLY) {
		collectionNode->addChild(match(TOK_OPEN_CURLY, "expected an array"));
		collectionNode->addChild(expression()); // ניתוח הביטוי הראשון
		while (currentToken().typeToken == TOK_COMMA) {
			collectionNode->addChild(match(TOK_COMMA));
			collectionNode->addChild(expression()); // ניתוח הביטוי הבא
		}
		collectionNode->addChild(match(TOK_CLOSE_CURLY, "Expected closing bracket for array"));
		return collectionNode;
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::static_array_declaration(Token &t) {
	// דקדוק: <static_array_declaration> ::= <type> TOK_ID TOK_LEFT_ARRAY <number> TOK_RIGHT_ARRAY
	shared_ptr<ParentNode> node = make_shared<ParentNode>("static_array_declaration",STATIC_ARRAY_DECLARATION);
	node->addChild(make_shared<TokenNode>(t));
	node->addChild(match(TOK_LEFT_ARRAY, "Expected '[' in array declaration"));
	node->addChild(number());
	node->addChild(match(TOK_RIGHT_ARRAY, "Expected ']' in array declaration"));
	if (currentToken().typeToken == TOK_ASSIGN)
	{
		node->addChild(match(TOK_ASSIGN));
		node->addChild(full_array());
	}
	return node;
}
shared_ptr<ASTNode> SyntacticAnalysis::dynamic_array_declaration(Pattern typeVariable) {
	// דקדוק: <dynamic_array_declaration> ::= <type> TOK_OPEN_BRACKET TOK_CLOSE_BRACKET TOK_ID TOK_ASSIGN TOK_NEW TOK_OPEN_BRACKET <expression> TOK_CLOSE_BRACKET
	shared_ptr<ParentNode> node = make_shared<ParentNode>("dynamic_array_declaration",DYNAMIC_ARRAY_DECLARATION);
	//node->addChild(make_shared<TokenNode>(typeVariable));
	node->addChild(match(TOK_LEFT_ARRAY, "Expected '[' after type for dynamic array"));
	node->addChild(match(TOK_RIGHT_ARRAY, "Expected ']' after '[' in dynamic array declaration"));
	node->addChild(match(TOK_ID, "Expected array identifier"));
	node->addChild(match(TOK_ASSIGN, "Expected '=' in dynamic array declaration"));
	node->addChild(match(TOK_NEW, "Expected 'new' keyword in dynamic array declaration"));
	node->addChild(match(typeVariable, "Expected type after 'new' keyword"));
	node->addChild(match(TOK_LEFT_ARRAY, "Expected '[' after new keyword and type"));
	node->addChild(expression());
	node->addChild(match(TOK_RIGHT_ARRAY, "Expected ']' after array size expression"));
	return node;
}
shared_ptr<ASTNode> SyntacticAnalysis::pointer_array_declaration() {
	// דקדוק: <pointer_array_declaration> ::= <type> TOK_STAR TOK_ID TOK_ASSIGN <allocation_function_call>
	shared_ptr<ParentNode> node = make_shared<ParentNode>("pointer_array_declaration",POINTER_ARRAY_DECLARATION);
	//node->addChild(type());
	node->addChild(match(TOK_ASTERISK, "Expected '*' for pointer declaration"));
	node->addChild(match(TOK_ID, "Expected identifier for pointer array declaration"));
	node->addChild(match(TOK_ASSIGN, "Expected '=' in pointer array declaration"));
	node->addChild(allocation_function_call());
	return node;
}
shared_ptr<ASTNode> SyntacticAnalysis::allocation_function_call() {
	// דקדוק:
	// <allocation_function_call> ::= TOK_MALLOC TOK_OPEN_PAREN <expression> TOK_CLOSE_PAREN
	//                              | TOK_CALLOC TOK_OPEN_PAREN <expression> TOK_COMMA <expression> TOK_CLOSE_PAREN
	//                              | TOK_REALLOC TOK_OPEN_PAREN <expression> TOK_COMMA <expression> TOK_CLOSE_PAREN
	shared_ptr<ParentNode> node = make_shared<ParentNode>("allocation_function_call",ALLOCATION_FUNCTION_CALL);
	Pattern p = currentToken().typeToken;
	if (p == TOK_MALLOC) {
		node->addChild(match(TOK_MALLOC, "Expected 'malloc'"));
		node->addChild(match(TOK_OPEN_PAREN, "Expected '(' after malloc"));
		node->addChild(expression());
		node->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after malloc argument"));
	}
	else if (p == TOK_CALLOC) {
		node->addChild(match(TOK_CALLOC, "Expected 'calloc'"));
		node->addChild(match(TOK_OPEN_PAREN, "Expected '(' after calloc"));
		node->addChild(expression());
		node->addChild(match(TOK_COMMA, "Expected ',' between calloc arguments"));
		node->addChild(expression());
		node->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after calloc arguments"));
	}
	else if (p == TOK_REALLOC) {
		node->addChild(match(TOK_REALLOC, "Expected 'realloc'"));
		node->addChild(match(TOK_OPEN_PAREN, "Expected '(' after realloc"));
		node->addChild(expression());
		node->addChild(match(TOK_COMMA, "Expected ',' between realloc arguments"));
		node->addChild(expression());
		node->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after realloc arguments"));
	}
	else {
		match(TOK_ERROR,"Expected allocation function (malloc, calloc, or realloc)");
	}
	return node;
}
shared_ptr<ASTNode> SyntacticAnalysis::string_declaration() {
	// דקדוק: <string_declaration> ::=
	//   TOK_STRING_TYPE TOK_ID TOK_ASSIGN (TOK_STRING_LITERAL 
	//        | (TOK_NEW TOK_STRING TOK_OPEN_PAREN TOK_STRING_LITERAL TOK_CLOSE_PAREN))
	shared_ptr<ParentNode> node = make_shared<ParentNode>("string_declaration",STRING_DECLARATION);
	node->addChild(match(TOK_STRING_TYPE, "Expected string type"));
	node->addChild(match(TOK_ID, "Expected identifier for string declaration"));
	node->addChild(match(TOK_ASSIGN, "Expected '=' in string declaration"));

	if (currentToken().typeToken == TOK_NEW) {
		node->addChild(match(TOK_NEW, "Expected 'new' in string declaration"));
		node->addChild(match(TOK_STRING, "Expected 'string' after new"));
		node->addChild(match(TOK_OPEN_PAREN, "Expected '(' after new string"));
		node->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
		node->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after string literal"));
	}
	else
	{
		node->addChild(match(TOK_STRING_LITERAL, "Expected string literal or new string constructor in string declaration"));
		node->addChild(match(TOK_STRING, "Excepted string after string literal"));
		node->addChild(match(TOK_STRING_LITERAL, "Expected string literal after string"));
	}
	node->addChild(match(TOK_SEMICOLON, "Excepted a ; after a statement"));
	return node;
}
// עדכון פונקציה statement
shared_ptr<ASTNode> SyntacticAnalysis::statement() {
	Pattern p = currentToken().typeToken;
	switch (p) {
	case TOK_IF:
		return if_else_statement();
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
	case TOK_LONG_TYPE:
		return declaration();
	case TOK_STRING_TYPE:
		return string_declaration();
	case TOK_VOID:
		return function_definition();
	case TOK_ID:
		return assignment();
	case TOK_OPEN_CURLY:
		return block();
	default:
		throw runtime_error("Unexpected token in statement");
	}
}
// ניתוח השמה-assignment--
shared_ptr<ASTNode> SyntacticAnalysis::parse() 
{
	shared_ptr<ParentNode> ast = make_shared<ParentNode>("program");
	try {
		while (currentTokenIndex < tokens.size() - 1) {
			ast->addChild(statement());
		}
		return ast;
	}
	catch (string s) {
		cerr << s << endl;
	}
}
void SyntacticAnalysis::printASTNodes(const shared_ptr<ASTNode>& node)
{
	node->printASTNode();
}
SyntacticAnalysis::~SyntacticAnalysis() {
}

