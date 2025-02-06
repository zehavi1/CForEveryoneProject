#pragma once
#include "SemanticAnalyzer.h"

class CodeGenerator
{
	shared_ptr<ASTNode> ast;
	//vector<map<string, Variable>> scopesFinal;
	//vector<map<string, Variable>> scopes;
	map<string, Variable> currentScope;
	int indexScope = 0;
public:
	CodeGenerator(shared_ptr<ASTNode>& ast) :ast(ast) {};
	shared_ptr<ASTNode>& getNewAst() { return ast; }
	void generateCode(shared_ptr<ASTNode>& node) {
		try {

			if (auto parentNode = dynamic_pointer_cast<ParentNode>(node)) {

				if (parentNode->name == "print")
				{
					generate_print_statement(parentNode);
				}
				else
					if (parentNode->name == "declaration")
					{
						generate_declaration(parentNode);
					}


					else
						if (parentNode->name == "for" || parentNode->name == "foreach" || parentNode->name == "while")
						{
							auto prevScope = currentScope;
							currentScope = parentNode->variableScope;

							if (parentNode->name == "foreach") {}
							//declareVariableInForeach(parentNode);
							int count = 0;
							for (auto& child : parentNode->children) {
								if (count < 5)
									count++;
								else
									generateCode(child); // ניתוח ילד
							}
							currentScope = prevScope;
						}
						else if (parentNode->name == "block")
						{
							auto prevScope = currentScope;
							currentScope = parentNode->variableScope;
							for (auto& child : parentNode->children) {
								generateCode(child); // ניתוח ילד
							}
							currentScope = prevScope;
						}
						else
							for (auto& child : parentNode->children) {
								generateCode(child); // ניתוח ילד
							}
			}

			else

				if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node))
				{
					if (auto binOpNodeleft = dynamic_pointer_cast<BinaryOpNode>(binOpNode->left))
					{
						if (binOpNodeleft->name == "range")
						{
							generate_ifrange(binOpNodeleft);

						}
						binOpNode->left = binOpNodeleft;
					}
					if (auto binOpNoderight = dynamic_pointer_cast<BinaryOpNode>(binOpNode->right))
					{
						if (binOpNoderight->name == "range")
						{
							generate_ifrange(binOpNoderight);

						}
						binOpNode->right = binOpNoderight;
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
	void generate_ifrange(shared_ptr<BinaryOpNode>& node)
	{
		auto first = dynamic_pointer_cast<BinaryOpNode>(node->left);
		shared_ptr<ASTNode>& second = node->right;
		shared_ptr<ASTNode>& exp = first->right;
		Token op = node->op;
		shared_ptr<BinaryOpNode> new_node_second = make_shared<BinaryOpNode>("less", op, exp, second);
		Token andToken(TOK_AND, "&&", op.lineNumber);
		shared_ptr<BinaryOpNode> new_node = make_shared<BinaryOpNode>("and", andToken, first, new_node_second);
		node = new_node;
	}
	void generate_print_statement(shared_ptr<ParentNode>& node)
	{
		std::map<Pattern, std::string> typesPrint = {
	{TOK_INT,"%d"},
	{TOK_CHAR,"%c"},
	{TOK_BOOL,"%s"},
	{TOK_DOUBLE,"%lf"},
	{TOK_FLOAT,"%f"},
	{TOK_LONG,"%ld"},
	{TOK_STRING,"%s"},
	{TOK_INT_TYPE,"%d"},
	{TOK_CHAR_TYPE,"%c"},
	{TOK_BOOL_TYPE,"%s"},
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
					string s = exprNode->printOriginalCode();
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


	//משתנים
	//in the middle---
	void generate_declaration(shared_ptr<ParentNode>& node)
	{
		const map<Pattern, string> mapsPetternTypes = {
	{TOK_BOOL_TYPE, "bool"},
	{TOK_CHAR_TYPE, "char"},
	{TOK_DOUBLE_TYPE, "double"},
	{TOK_FLOAT_TYPE, "float"},
	{TOK_INT_TYPE, "int"},
	{TOK_LONG_TYPE, "long"},
	{TOK_STRING_TYPE, "string"} };

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
	}
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

