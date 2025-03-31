#include "CodeGenerator.h"
const map<Pattern, string> mapsPetternTypes = {
{TOK_BOOL_TYPE, "bool"},
{TOK_CHAR_TYPE, "char"},
{TOK_DOUBLE_TYPE, "double"},
{TOK_FLOAT_TYPE, "float"},
{TOK_INT_TYPE, "int"},
{TOK_LONG_TYPE, "long"},
{TOK_STRING_TYPE, "string"} };
void CodeGenerator::generateCode(shared_ptr<ASTNode>& node) {
	try {

		if (auto parentNode = dynamic_pointer_cast<ParentNode>(node)) {
			switch (parentNode->nodeType)
			{
			case PRINT_STATEMENT:
			{
				generate_print_statement(parentNode);
				break;
			}
			case DECLARATION:
			{
				generate_declaration(parentNode);
				break;
			}
			case DYNAMIC_ARRAY_DECLARATION:
				generate_dynamic_array(parentNode);
				break;
			case ELIF_STATEMENT:
				generate_elif(parentNode);
				break;
			case FOREACH_LOOP:
			case FOR_LOOP:
			case WHILE_LOOP:
			{
				auto prevScope = currentScope;
				currentScope = parentNode->variableScope;
				int count = 0;
				if (parentNode->nodeType == FOREACH_LOOP)
					generate_foreach_loop(parentNode);
				for (auto& child : parentNode->children) {
					if (count < 5)
						count++;
					else
						generateCode(child); // ניתוח ילד
				}
				currentScope = prevScope;
				break;
			}
			case FUNCTION_DEFINITION:
			{

				auto prevScope = currentScope;
				currentScope = parentNode->variableScope;
				for (auto& child : parentNode->children) {
					generateCode(child); // ניתוח ילד
				}
				currentScope = prevScope;
				addFree(parentNode);
				break;
			}
			case PARAMETER_LIST:
				//////////
				break;
			case BLOCK:
			{
				auto prevScope = currentScope;
				currentScope = parentNode->variableScope;
				for (auto& child : parentNode->children) {
					generateCode(child); // ניתוח ילד
				}
				currentScope = prevScope;
				break;
			}
			default:
				for (auto& child : parentNode->children) {
					generateCode(child); // ניתוח ילד
				}
				break;
			}
		}
		else
			if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node))
			{
				if (binOpNode->nodeType == RANGE)
				{
					generate_ifrange(binOpNode);
				}
				generateCode(binOpNode->left);
				generateCode(binOpNode->right);
			}

	}
	catch (string s) {
		if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
			cout << "line " << varNode->token.lineNumber << ":" << s;
		}
		else
			cout << s;
	}


}
void CodeGenerator::addDefineBoolAndIncludes()
{
	bool isBool = false, isIO = false, isArray = false;;
	shared_ptr<ParentNode> basic = make_shared<ParentNode>("add code before");
	shared_ptr<SentenceNode> crt = make_shared<SentenceNode>("#define _CRT_SECURE_NO_WARNINGS\n");
	//shared_ptr<SentenceNode> forStrings = make_shared<SentenceNode>("#pragma warning (disable: 4996)\n");
	shared_ptr<SentenceNode> includeStdio = make_shared<SentenceNode>("#include <stdio.h>\n");
	shared_ptr<SentenceNode> insludeStdlib = make_shared<SentenceNode>("#include <stdlib.h>\n");
	shared_ptr<SentenceNode> defineBool = make_shared<SentenceNode>("#define bool {false,true};\n");
	shared_ptr<SentenceNode> addAlloc = make_shared<SentenceNode>("#define MAX_ALLOCATED 100\nvoid* allocated_arrays[MAX_ALLOCATED];\nint allocated_count = 0;\n");
	basic->addChild(crt);
	for (Token& t : tokens)
	{
		if (t.typeToken == TOK_BOOL || t.typeToken == TOK_BOOL_TYPE)
			isBool = true;
		else
			if (t.typeToken == TOK_PRINTF || t.typeToken == TOK_PRINT)
				isIO = true;
			else
				if (t.typeToken == TOK_LEFT_ARRAY || t.typeToken == TOK_MALLOC)
					isArray = true;
	}
	if (isBool)
		basic->addChild(defineBool);
	if (isIO)
		basic->addChild(includeStdio);
	if (isArray)
		basic->addChild(addAlloc);
	auto ast2 = dynamic_pointer_cast<ParentNode>(make_shared<ParentNode>("program with all"));
	ast2->addChild(basic);
	ast2->addChild(ast);
	ast = ast2;

}
void CodeGenerator::generate_dynamic_array(shared_ptr<ParentNode>& node)
{
	// דקדוק: <dynamic_array_declaration> ::= <type> TOK_OPEN_BRACKET TOK_CLOSE_BRACKET TOK_ID TOK_ASSIGN TOK_NEW TOK_OPEN_BRACKET <expression> TOK_CLOSE_BRACKET
	auto nameNode = dynamic_pointer_cast<TokenNode>(node->children[2]);
	string name = nameNode->token.value;
	Pattern typeArr = currentScope.at(name).token.typeToken;
	auto expr = node->children[7];
	//node->children.clear();
	vector<shared_ptr<ASTNode>> children;
	children.push_back(make_shared<TokenNode>(Token(TOK_ASTERISK, "*")));
	children.push_back(nameNode);
	children.push_back(make_shared<TokenNode>(Token(TOK_ASSIGN, "=")));
	children.push_back(make_shared<TokenNode>(Token(TOK_MALLOC, "malloc")));
	children.push_back(make_shared<TokenNode>(Token(TOK_OPEN_PAREN, "(")));
	children.push_back(expr);
	children.push_back(make_shared<TokenNode>(Token(TOK_ASTERISK, "*")));
	children.push_back(make_shared<TokenNode>(Token(TOK_ID, "sizeof(")));
	children.push_back(make_shared<TokenNode>(Token(typeArr, mapsPetternTypes.at(typeArr))));
	children.push_back(make_shared<TokenNode>(Token(TOK_CLOSE_PAREN, ")")));
	children.push_back(make_shared<TokenNode>(Token(TOK_CLOSE_PAREN, ")")));
	node->children = children;
}
void CodeGenerator::generate_foreach_loop(shared_ptr<ParentNode>& node)
{
	// דקדוק: <foreach_loop> ::= TOK_FOREACH TOK_OPEN_PAREN  (<TOK_TYPE>?)  TOK_ID TOK_IN <collection>, (TOK_ID | TOK_INT) TOK_CLOSE_PAREN <block>
	/*<foreach_loop> :: = TOK_FOREACH TOK_OPEN_PAREN  (<TOK_TYPE>?)  TOK_ID TOK_IN <collection>, (TOK_ID | TOK_INT) TOK_CLOSE_PAREN <block>
		<collection> :: = TOK_ID | <full_array>*/
	shared_ptr<TokenNode> typeNode = make_shared<TokenNode>(Token(TOK_INT, "int"));
	shared_ptr<TokenNode> nameNode;
	if (auto tokenNode = dynamic_pointer_cast<TokenNode>(node->children[2])) {
		nameNode = tokenNode;
	}
	auto collection = node->children[4];
	string nameCollection = "";
	auto size = node->children[6];
	shared_ptr<ParentNode> block = dynamic_pointer_cast<ParentNode>(node->children[8]);
	if (node->children[2]->nodeType != TOK_INT && node->children[2]->nodeType != TOK_ID)
	{
		typeNode = dynamic_pointer_cast<TokenNode>(node->children[2]);
		nameNode = dynamic_pointer_cast<TokenNode>(node->children[3]);
		collection = node->children[5];
		size = node->children[7];
		block = dynamic_pointer_cast<ParentNode>(node->children[9]);
	}
	if (auto tokenNode = dynamic_pointer_cast<TokenNode>(collection)) {
		nameCollection = tokenNode->token.value;
	}
	string nameIndex = nameNode->token.value+"Index";
	//node->children.clear();
	vector<shared_ptr<ASTNode>> children;
	children.push_back(make_shared<TokenNode>(Token(TOK_FOR, "for")));
	children.push_back(make_shared<TokenNode>(Token(TOK_OPEN_PAREN, "(")));
	children.push_back(make_shared<TokenNode>(Token(TOK_INT, "int")));
	children.push_back(make_shared<TokenNode>(Token(TOK_ID, nameIndex)));
	children.push_back(make_shared<TokenNode>(Token(TOK_ASSIGN, "=")));
	children.push_back(make_shared<TokenNode>(Token(TOK_INT, "0")));
	children.push_back(make_shared<TokenNode>(Token(TOK_SEMICOLON, ";")));
	children.push_back(make_shared<TokenNode>(Token(TOK_ID, nameIndex)));
	children.push_back(make_shared<TokenNode>(Token(TOK_LESS, "<")));
	children.push_back(size);
	children.push_back(make_shared<TokenNode>(Token(TOK_SEMICOLON, ";")));
	children.push_back(make_shared<TokenNode>(Token(TOK_ID, nameIndex)));
	children.push_back(make_shared<TokenNode>(Token(TOK_INCREMENT, "++")));
	children.push_back(make_shared<TokenNode>(Token(TOK_CLOSE_PAREN, ")")));
	shared_ptr<ParentNode> declaration = make_shared<ParentNode>("declaration",DECLARATION);
	declaration->addChild(typeNode);
	shared_ptr<ParentNode> variableList = make_shared<ParentNode>("variable_list", VARIABLE_LIST);
	shared_ptr<ParentNode> variable = make_shared<ParentNode>("variable", VARIABLE);
	variable->addChild(nameNode);
	variable->addChild(make_shared<TokenNode>(Token(TOK_ASSIGN, "=")));
	shared_ptr<ParentNode> expression = make_shared<ParentNode>("factor", FACTOR);
	expression->addChild(make_shared<TokenNode>(Token(TOK_ID, nameCollection)));
	expression->addChild(make_shared<TokenNode>(Token(TOK_LEFT_ARRAY, "[")));
	expression->addChild(make_shared<TokenNode>(Token(TOK_ID, nameIndex)));
	expression->addChild(make_shared<TokenNode>(Token(TOK_RIGHT_ARRAY, "]")));
	variable->addChild(expression);
	variableList->addChild(variable);
	declaration->addChild(variableList);
	declaration->addChild(make_shared<TokenNode>(Token(TOK_SEMICOLON, ";")));
	//shared_ptr<ParentNode> block2 = make_shared<ParentNode>("block");
	//block2->addChild(declaration);
	//block2->addChild(block);
	//children.push_back(block2);
	block->children.insert(block->children.begin()+1, declaration);
	children.push_back(block);
	node->children = children;

}
void CodeGenerator::generate_elif(shared_ptr<ParentNode>& node)
{
	shared_ptr<TokenNode> elseToken = make_shared<TokenNode>(Token(TOK_ELSE, "else", 1));
	shared_ptr<TokenNode> ifToken = make_shared<TokenNode>(Token(TOK_IF, "if", 1));
	shared_ptr<ParentNode> elseifNode = make_shared<ParentNode>("else-if");
	elseifNode->addChild(elseToken);
	elseifNode->addChild(ifToken);
	node->changeChild(elseifNode, 0);
	int i = 0;
	for (auto& child : node->children)
	{
		if (i++ > 0)
			generateCode(child);
	}
}
shared_ptr<ParentNode>& CodeGenerator::insertAfter(shared_ptr<ASTNode>& node, shared_ptr<ASTNode>& nodeToInsert)
{
	shared_ptr<ParentNode> newNode = make_shared<ParentNode>("together");
	newNode->addChild(node);
	newNode->addChild(nodeToInsert);
	return newNode;
}
void CodeGenerator::generate_ifrange(shared_ptr<BinaryOpNode>& node)
{
	auto first = dynamic_pointer_cast<BinaryOpNode>(node->left);
	shared_ptr<ASTNode>& second = node->right;
	shared_ptr<ASTNode>& exp = first->right;
	Token op = node->op;
	shared_ptr<BinaryOpNode> new_node_second = make_shared<BinaryOpNode>("and", op, exp, second, EXPR_AND);
	Token andToken(TOK_AND, "&&", op.lineNumber);
	node->name = "and";
	node->op = andToken;
	node->changeChild(first, 0);
	node->changeChild(new_node_second, 1);
}
void CodeGenerator::generate_print_statement(shared_ptr<ParentNode>& node)
{
	std::map<Pattern, std::string> typesPrint = {
{TOK_INT,"%d"},
{TOK_CHAR,"%c"},
{TOK_BOOL,"%d"},
{TOK_DOUBLE,"%lf"},
{TOK_FLOAT,"%f"},
{TOK_LONG,"%ld"},
{TOK_STRING,"%s"},
{TOK_INT_TYPE,"%d"},
{TOK_CHAR_TYPE,"%c"},
{TOK_BOOL_TYPE,"%d"},
{TOK_DOUBLE_TYPE,"%lf"},
{TOK_FLOAT_TYPE,"%f"},
{TOK_LONG_TYPE,"%ld"},
{TOK_STRING_TYPE,"%s"}
	};
	shared_ptr<ParentNode> print_node = dynamic_pointer_cast<ParentNode>(node);
	shared_ptr<ParentNode> exprPrint_node = dynamic_pointer_cast<ParentNode>(print_node->children[2]);
	shared_ptr<ParentNode> printf_node = make_shared<ParentNode>("printf_expr_statement");
	Token t = dynamic_pointer_cast<TokenNode>(print_node->children[0])->token;
	print_node->changeChild(make_shared<TokenNode>(Token(TOK_PRINTF, "printf", t.lineNumber)), 0);
	//printf_node->addChild(make_shared<TokenNode>(Token( TOK_PRINTF,"printf",t.lineNumber)));
	//printf_node->addChild(print_node->children[1]);
	string s = "\"";
	printf_node->addChild(make_shared<TokenNode>(Token(TOK_STRING_LITERAL, s, t.lineNumber)));
	vector<Variable> v;
	for (auto x : exprPrint_node->children)
	{
		Pattern p;
		if (auto exprNode = dynamic_pointer_cast<BinaryOpNode>(x)) {
			if (exprNode->name == "expressionInPrint")
			{
				string s = exprNode->printOriginalCode(0);
				printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%lf", t.lineNumber)));
				Token t2(TOK_DOUBLE, s, t.lineNumber);
				v.push_back(Variable(t2));
				//hear i need insert code to put all expr node as a variable to push the vector.
			}
		}
		else
			if (auto tokNode = dynamic_pointer_cast<TokenNode>(x))
			{
				p = tokNode->token.typeToken;
				switch (p)
				{
				case TOK_ID:
				{
					p = currentScope[tokNode->token.value].token.typeToken;
					//if (p == TOK_STRING_TYPE)
					//{
					//	printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%s", t.lineNumber)));
					//	//printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					//	v.push_back(Variable(tokNode->token));
					//	break;
					//}

				}
				case TOK_STRING_TYPE:
				case TOK_INT_TYPE:
				case TOK_CHAR_TYPE:
				case TOK_LONG_TYPE:
				case TOK_FLOAT_TYPE:
				case TOK_DOUBLE_TYPE:
				case TOK_INT:
				case TOK_CHAR:
				case TOK_LONG:
				case TOK_FLOAT:
				case TOK_DOUBLE:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, typesPrint[p], t.lineNumber)));
					v.push_back(Variable(tokNode->token));
					break;
				}
				case TOK_STRING:
				{
					printf_node->addChild(tokNode);
					break;
				}
				default:
					break;
				}
			}

	}
	printf_node->addChild(make_shared<TokenNode>(Token(TOK_STRING_LITERAL, s, t.lineNumber)));


	//printf_node->addChild(make_shared<TokenNode>(TOK_STRING_LITERAL,s,t.lineNumber));

	for (Variable& x : v)
	{
		printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
		printf_node->addChild(make_shared<TokenNode>(x.token));
	}
	//printf_node->addChild(make_shared<TokenNode>(TOK_STRING_LITERAL, s, t.lineNumber));
	print_node->changeChild(printf_node, 2);

}

// פונקציה שמוסיפה קוד לאחר הכרזת מערך – היא מוסיפה שורה שמכניסה את המצביע שהוקצה לרשימה הגלובלית.
void CodeGenerator::addAllocAfterArrayDeclaration(shared_ptr<ParentNode>& node, map<string, Variable> variables) {
	shared_ptr<ParentNode> p = make_shared<ParentNode>("allocate");
	for (const auto& entry : variables) {
		const Variable& var = entry.second;
		if (var.isArray == true) {
			// לדוגמה: "allocated_arrays[allocated_count++] = arrName;"
			string s = "allocated_arrays[allocated_count++] = " + var.token.value + ";\n";
			shared_ptr<SentenceNode> alloc = make_shared<SentenceNode>(s);
			p->addChild(alloc);
		}
	}
	node->addChild(p);
	// יש להוסיף את הצומת p לעץ הקוד הגלובלי (לדוגמה, בסוף הפונקציה main)
	// לדוגמה: globalAST->addChild(p);
}
void CodeGenerator::addFree(shared_ptr<ParentNode>& a) {
	string s = "for (int i = 0; i < allocated_count; i++) {\n"
		"    free(allocated_arrays[i]);\n"
		"}\n";
	shared_ptr<ParentNode> freeNode = make_shared<ParentNode>("free");
	shared_ptr<SentenceNode> freeSentence = make_shared<SentenceNode>(s);
	freeNode->addChild(freeSentence);
	if (a->nodeType == FUNCTION_DEFINITION)
	{
		auto node1 = dynamic_pointer_cast<ParentNode>(a->children[5]);
		auto x = node1->children.back();
		node1->children.back() = freeNode;
		node1->addChild(x);
	}
}
// פונקציה שמוסיפה קוד לשחרור כל ההקצאות הדינאמיות בסיום התוכנית.
//void CodeGenerator::addFree() {
//	// הקוד שיופק – לולאה שעוברת על כל המצביעים שהוקצו ונעשית free להם.
//	string s = "for (int i = 0; i < allocated_count; i++) {\n"
//		"    free(allocated_arrays[i]);\n"
//		"}\n";
//	shared_ptr<ParentNode> freeNode = make_shared<ParentNode>("free");
//	shared_ptr<SentenceNode> freeSentence = make_shared<SentenceNode>(s);
//	freeNode->addChild(freeSentence);
//
//	// יש להוסיף את הצומת freeNode לעץ הקוד הגלובלי, למשל בסיום הפונקציה main.
//	// לדוגמה:
//	// globalAST->addChild(freeNode);
//	auto ast2 = dynamic_pointer_cast<ParentNode>(ast);
//	for (auto& a : ast2->children)
//	{
//		if (a->nodeType == FUNCTION_DEFINITION)
//		{
//			auto node1 = dynamic_pointer_cast<ParentNode>(a);
//			auto& x = node1->children.back();
//			node1->children.back() = freeNode;
//			node1->addChild(x);
//		}
//	}
//}

//משתנים
//in the middle---
void CodeGenerator::generate_declaration(shared_ptr<ParentNode>& node)
{


	shared_ptr<ParentNode> dec_node = dynamic_pointer_cast<ParentNode>(node);
	shared_ptr<TokenNode> type_node = dynamic_pointer_cast<TokenNode>(dec_node->children[0]);
	if (type_node->token.typeToken == TOK_VAR)
	{
		shared_ptr<ParentNode> var_list = dynamic_pointer_cast<ParentNode>(dec_node->children[1]);
		shared_ptr<ParentNode> var_first = dynamic_pointer_cast<ParentNode>(var_list->children[0]);
		shared_ptr<TokenNode> type_real_node = dynamic_pointer_cast<TokenNode>(var_first->children[0]);
		Pattern typeReal = currentScope[type_real_node->token.value].token.typeToken;
		type_node->token.typeToken = typeReal;
		type_node->token.value = mapsPetternTypes.at(typeReal);
	}
	auto varList = dynamic_pointer_cast<ParentNode>(node->children[1])->children;
	for (auto& child : varList)
		if (child->nodeType == DYNAMIC_ARRAY_DECLARATION)
			generateCode(child);
}
