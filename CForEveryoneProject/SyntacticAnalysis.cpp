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
//נבדק בהצלחה
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
// פונקציה לניתוח מספרים
//נבדק בהצלחה
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
// הוספת פונקציה לניתוח בלוקים

shared_ptr<ASTNode> SyntacticAnalysis::block()
{
	shared_ptr<ParentNode> blockNode = make_shared<ParentNode>("block");
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
//נבדק בהצלחה
shared_ptr<ASTNode> SyntacticAnalysis::print_statement() {
	shared_ptr<ParentNode> printNode = make_shared<ParentNode>("print");
	printNode->addChild(match(TOK_PRINT)); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after print")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(expr_print()); // להתאים את הביטוי להדפסה
	printNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after expression")); // להתאים את הטוקן הנוכחי לפונקציה
	printNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement")); // להתאים את הטוקן הנוכחי לפונקציה
	return printNode; // החזרת צומת הדפסה
}
// פונקציה לניתוח הביטויים להדפסה
//נבדק בהצלחה
shared_ptr<ASTNode> SyntacticAnalysis::expr_print() {
	shared_ptr<ParentNode> exprNode = make_shared<ParentNode>("expr_print");
	// ניתוח הביטוי הראשון או המחרוזת
	if (currentToken().typeToken == TOK_STRING_LITERAL) {
		exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
		exprNode->addChild(match(TOK_STRING, "Expected string"));
		exprNode->addChild(match(TOK_STRING_LITERAL, "Expected string literal"));
	}
	else {
		exprNode->addChild(expression()); // ניתוח ביטוי
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
			exprNode->addChild(expression()); // ניתוח ביטוי נוסף
		}
	}

	return exprNode; // החזרת צומת הביטוי להדפסה
}
//פונקציות לניתוח תנאים-עובדות טוב אבל יש באג
//אם יש סוגריים של ביטויים זה לא עובד
shared_ptr<ASTNode> SyntacticAnalysis::conditions() {
	shared_ptr<ParentNode> conditionNode = make_shared<ParentNode>("conditions");
	conditionNode->addChild(condition()); // ניתוח הביטוי הראשון
	while (currentToken().typeToken == TOK_AND || currentToken().typeToken == TOK_OR) {
		conditionNode->addChild(logical_operator()); // ניתוח אופרטור לוגי
		conditionNode->addChild(condition());
	}
	return conditionNode; // החזרת צומת תנאי
}
shared_ptr<ASTNode> SyntacticAnalysis::condition() {
	shared_ptr<ParentNode> conditionNode = make_shared<ParentNode>("condition");
	int current = currentTokenIndex;
	Pattern p;
	if (currentToken().typeToken == TOK_OPEN_PAREN)
	{
		conditionNode->addChild(match(TOK_OPEN_PAREN));
		conditionNode->addChild(conditions());
		conditionNode->addChild(match(TOK_CLOSE_PAREN));
	}
	else
	{
		conditionNode->addChild(expression()); // ניתוח הביטוי הראשון
		p = currentToken().typeToken;
		conditionNode->addChild(comparison_operator()); // ניתוח אופרטור השוואה
		conditionNode->addChild(expression()); // ניתוח הביטוי השני

		if (currentToken().typeToken == p)
		{
			conditionNode->name = "condition-range";
			conditionNode->addChild(comparison_operator()); // ניתוח אופרטור השוואה
			conditionNode->addChild(expression()); // ניתוח הביטוי השני
		}
	}
	
	
	return conditionNode; // החזרת צומת תנאי
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
shared_ptr<ASTNode> SyntacticAnalysis::logical_operator() {
	Pattern p = currentToken().typeToken;
	if (p == TOK_AND || p == TOK_OR) {
		return match(p);
	}
	else {
		match(TOK_EOF, "Expected logical operator");
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::elif_statement() {
	shared_ptr<ParentNode> ifElseIfNode = make_shared<ParentNode>("elif");
	ifElseIfNode->addChild(match(TOK_ELIF));
	ifElseIfNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after elif"));
	ifElseIfNode->addChild(condition());
	ifElseIfNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after condition"));
	ifElseIfNode->addChild(block());
	return ifElseIfNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::if_else_statement() {
	shared_ptr<ParentNode> ifElseNode = make_shared<ParentNode>("if_else");
	ifElseNode->addChild(match(TOK_IF));
	ifElseNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after if"));
	ifElseNode->addChild(conditions());
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
	declarationNode->addChild(match(TOK_SEMICOLON, "Expected ';' after variable declaration")); // לבדוק אם יש נקודה-פסיק
	return declarationNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::declaration1() {
	shared_ptr<ParentNode> declarationNode = make_shared<ParentNode>("declaration");
	Pattern typeVariable = currentToken().typeToken;
	if (currentToken().typeToken == TOK_VAR)
	{
		declarationNode->addChild(match(TOK_VAR)); // לבדוק אם יש מילת מפתח var
	}
	else
	{
		declarationNode->addChild(type()); // ניתוח סוג המשתנה 
	}
	
	declarationNode->addChild(variable_list(typeVariable)); // ניתוח רשימת המשתנים
	//declarationNode->addChild(match(TOK_SEMICOLON, "Expected ';' after variable declaration")); // לבדוק אם יש נקודה-פסיק

	return declarationNode; // החזרת צומת הצהרת משתנים
}

// פונקציה לניתוח רשימת משתנים
shared_ptr<ASTNode> SyntacticAnalysis::variable_list(Pattern typeVariable) {
	shared_ptr<ParentNode> variableListNode = make_shared<ParentNode>("variable_list");
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
	shared_ptr<ParentNode> variableNode = make_shared<ParentNode>("variable");
	variableNode->addChild(match(TOK_ID)); // ניתוח מזהה המשתנה
	Token t = currentToken().typeToken;
	/*if(t.typeToken==TOK_LEFT_ARRAY)
		array_declaretion()*/
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
	shared_ptr<ParentNode> varDeclNode = make_shared<ParentNode>("variable_declaration");
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
//עד כאן נבדק בהצלחה
/////////////////////////////////////////////////////////////////////////////////////
shared_ptr<ASTNode> SyntacticAnalysis::assignment() {
    shared_ptr<ParentNode> startNode = dynamic_pointer_cast<ParentNode>(assignment1());
    if (!startNode) {
        throw runtime_error("Failed to cast ASTNode to ParentNode");
    }
	startNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
	return startNode;
}
shared_ptr<ASTNode> SyntacticAnalysis::assignment1() {
	shared_ptr<ParentNode> startNode = make_shared<ParentNode>("assignment");
	if (currentToken().typeToken != TOK_ID)
		return declaration1();
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
// הוספת פונקציות לניתוח לולאות
shared_ptr<ASTNode> SyntacticAnalysis::for_loop() {
	shared_ptr<ParentNode> forNode = make_shared<ParentNode>("for");
	forNode->addChild(match(TOK_FOR));
	forNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after for"));
	forNode->addChild(assignment());
	//forNode->addChild((match(TOK_SEMICOLON, "Expected ';' after assignment")));
	forNode->addChild(condition());
	forNode->addChild(match(TOK_SEMICOLON, "Expected ';' after condition"));
	forNode->addChild(assignment1());
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
	Token idToken = currentToken(); // טוקן עבור מזהה
	if (idToken.typeToken == TOK_ID)
		return match(TOK_ID, "Expected identifier in collection");
	else
	{
		shared_ptr<ParentNode> collectionNode = make_shared<ParentNode>("collection");
		if (currentToken().typeToken == TOK_LEFT_ARRAY) {
			collectionNode->addChild(match(TOK_LEFT_ARRAY, "expected an array"));
			collectionNode->addChild(expression()); // ניתוח הביטוי הראשון
			while (currentToken().typeToken == TOK_COMMA) {
				nextToken(); // לעבור לטוקן הבא
				collectionNode->addChild(expression()); // ניתוח הביטוי הבא
			}
			collectionNode->addChild(match(TOK_RIGHT_ARRAY, "Expected closing bracket for array"));
				return collectionNode;
		}
	}
}
shared_ptr<ASTNode> SyntacticAnalysis::foreach_loop() {
	shared_ptr<ParentNode> foreachNode = make_shared<ParentNode>("foreach");
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
	foreachNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after foreach"));
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
	case TOK_STRING_TYPE:
	case TOK_LONG_TYPE:
		return declaration();
	case TOK_ID:
		return assignment();
	case TOK_OPEN_CURLY:
		return block();
	default:
		throw runtime_error("Unexpected token in statement");
	}
}

// ניתוח השמה-assignment--לא לשכוח לעשות גם לאתחול משתנים

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

