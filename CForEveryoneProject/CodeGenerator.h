#pragma once
#include "SemanticAnalyzer.h"

class CodeGenerator
{
	shared_ptr<ASTNode> ast;
	map<string, Variable> currentScope;
	vector<Token> tokens;
	int indexScope = 0;
public:
	CodeGenerator(shared_ptr<ASTNode>& ast) :ast(ast) {};
	CodeGenerator(shared_ptr<ASTNode>& ast, vector<Token>& tokens) :ast(ast),tokens(tokens) {};
	void CodeGenerator_main() {
		addDefineBoolAndIncludes();
		generateCode(ast);
	}
	shared_ptr<ASTNode>& getNewAst() { return ast; }
	void generateCode(shared_ptr<ASTNode>& node);
	void generate_elif(shared_ptr<ParentNode>& node);
	shared_ptr<ParentNode>& insertAfter(shared_ptr<ASTNode>& node, shared_ptr<ASTNode>& nodeToInsert);
	void generate_ifrange(shared_ptr<BinaryOpNode>& node);
	void generate_print_statement(shared_ptr<ParentNode>& node);
	void generate_dynamic_array(shared_ptr<ParentNode>& node)
	{
		// דקדוק: <dynamic_array_declaration> ::= <type> TOK_OPEN_BRACKET TOK_CLOSE_BRACKET TOK_ID TOK_ASSIGN TOK_NEW TOK_OPEN_BRACKET <expression> TOK_CLOSE_BRACKET
		auto nameNode = dynamic_pointer_cast<TokenNode>(node->children[2]);
		string name = nameNode->token.value;
		Pattern typeArr = currentScope.at(name).token.typeToken;
		auto expr = node->children[7];
		//node->children.clear();
		vector<shared_ptr<ASTNode>> children;
		children.push_back(make_shared<TokenNode>( Token( TOK_ASTERISK, "*")));
		children.push_back(nameNode);
		children.push_back(make_shared<TokenNode>( Token( TOK_ASSIGN, "=")));
		children.push_back(make_shared<TokenNode>( Token( TOK_MALLOC, "malloc")));
		children.push_back(make_shared<TokenNode>(Token(TOK_OPEN_PAREN, "(")));
		children.push_back(expr);
		children.push_back(make_shared<TokenNode>( Token( TOK_CLOSE_PAREN, ")")));
		node->children = children;
	}
	void addDefineBoolAndIncludes()
	{
		bool isBool = false, isIO = false;
		shared_ptr<ParentNode> basic = make_shared<ParentNode>("add code before");
		shared_ptr<SentenceNode> crt = make_shared<SentenceNode>("#define _CRT_SECURE_NO_WARNINGS\n");
		//shared_ptr<SentenceNode> forStrings = make_shared<SentenceNode>("#pragma warning (disable: 4996)\n");
		shared_ptr<SentenceNode> includeStdio = make_shared<SentenceNode>("#include <stdio.h>\n");
		shared_ptr<SentenceNode> insludeStdlib = make_shared<SentenceNode>("#include <stdlib.h>\n");
		shared_ptr<SentenceNode> defineBool = make_shared<SentenceNode>("define bool{false,true};\n");
		basic->addChild(crt);
		for (Token &t: tokens)
		{
			if (t.typeToken == TOK_BOOL|| t.typeToken == TOK_BOOL_TYPE)
				isBool = true;
			else
				if (t.typeToken == TOK_PRINTF||t.typeToken==TOK_PRINT)
					isIO = true;
		}
		if (isBool)
			basic->addChild(defineBool);
		if (isIO)
			basic->addChild(includeStdio);
		auto ast2 =dynamic_pointer_cast<ParentNode>( make_shared<ParentNode>("program with all"));
		ast2->addChild(basic);
		ast2->addChild(ast);
		ast = ast2;
		
	}
	//משתנים
	void generate_declaration(shared_ptr<ParentNode>& node);
	//////////////////////////////////////////////////////////////////////
	shared_ptr<ASTNode> declaration1();
	shared_ptr<ASTNode> variable(Pattern typeVariable); // ניתוח משתנה
	shared_ptr<ASTNode> variable_list(Pattern typeVariable); // ניתוח מזהה

	//משפטי השמה והדפסה
	shared_ptr<ASTNode> print_statement();
	shared_ptr<ASTNode> variable_declaration(); // ניתוח הצהרות משתנים
	shared_ptr<ASTNode> expr_print();
	shared_ptr<ASTNode> assignment(); // ניתוח השמות
	shared_ptr<ASTNode> assignment1();
	shared_ptr<ASTNode> statement(); // ניתוח פקודות
	//פקודות if
	shared_ptr<ASTNode> if_else_statement(); // ניתוח פקודות if-else
	shared_ptr<ASTNode> elif_statement(); // ניתוח פקודות if-else-if
	shared_ptr<ASTNode> logical_condition();
	shared_ptr<ASTNode> choose_condition();
	shared_ptr<ASTNode> logical_condition_inner();
	shared_ptr<ASTNode> math_condition();
	shared_ptr<ASTNode> if_else_statement2();
	Token peekNextToken();
	shared_ptr<ASTNode> conditions();
	shared_ptr<ASTNode> conditions_inner();
	shared_ptr<ASTNode> conditions2(); // ניתוח תנאים
	shared_ptr<ASTNode> condition2(); // ניתוח תנאי
	shared_ptr<ASTNode> comparison_operator(); // ניתוח אופרטורים להשוואה
	shared_ptr<ASTNode> logical_operator(); // ניתוח אופרטורים לוגיים
	shared_ptr<ASTNode> block(); // ניתוח בלוקים

	//לולאות
	shared_ptr<ASTNode> generate_foreach_loop(); // ניתוח לולאות foreach
	shared_ptr<ASTNode> generate_function_definition(); // ניתוח הגדרות פונקציות
	shared_ptr<ASTNode> generate_collection(); // ניתוח אוספים

};

