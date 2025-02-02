#pragma once
#include "SemanticAnalyzer.h"
class CodeGenerator
{
	shared_ptr<ASTNode> ast;
	vector<map<string, Variable>> scopesFinal;
	vector<map<string, Variable>> scopes;
	int indexScope=0;
public:
	CodeGenerator(shared_ptr<ASTNode>& ast, vector<map<string, Variable>> scopesFinal) :ast(ast),scopesFinal(scopesFinal) {};
	shared_ptr<ASTNode>& getNewAst() { return ast; }
	void generateCode(shared_ptr<ASTNode> node) {
		try {
			if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
				switch (varNode->token.typeToken)
				{
				case TOK_ID: {
					//useVariable(varNode->token.value);
				}
						   break;
						   /*case TOK_OPEN_CURLY:
						   { enterScope(); }
						   break;
						   case TOK_CLOSE_CURLY: { exitScope(); }
											   break;*/
				default:
					break;
				}

			}
			else if (auto parentNode = dynamic_pointer_cast<ParentNode>(node)) {
				// ניתוח צומת אב רגיל
				if (parentNode->name == "print")
				{
					generate_print_statement(parentNode);
				}
				//declareVariable(parentNode);
				else
					if (parentNode->name == "for" || parentNode->name == "foreach" || parentNode->name == "while")
					{
						indexScope++;
						if (parentNode->name == "foreach"){}
							//declareVariableInForeach(parentNode);
						int count = 0;
						for (auto& child : parentNode->children) {
							if (count < 5)
								count++;
							else
								generateCode(child); // ניתוח ילד
						}
						indexScope--;
					}
					else if (parentNode->name == "block")
					{
						indexScope++;
						for (auto& child : parentNode->children) {
							generateCode(child); // ניתוח ילד
						}
						indexScope--;
					}
					else
						for (auto& child : parentNode->children) {
							generateCode(child); // ניתוח ילד
						}
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
	void generate_print_statement(shared_ptr<ASTNode> node)
	{
		shared_ptr<ParentNode> print_node = dynamic_pointer_cast<ParentNode>(node);
		shared_ptr<ParentNode> exprPrint_node = dynamic_pointer_cast<ParentNode>(print_node->children[2]);
		shared_ptr<ParentNode> printf_node= make_shared<ParentNode>("printf_expr_statement");
		Token t = dynamic_pointer_cast<TokenNode>(print_node->children[0])->token;
		print_node->changeChild(make_shared<TokenNode>(Token( TOK_PRINTF,"printf",t.lineNumber)),0);
		//printf_node->addChild(make_shared<TokenNode>(Token( TOK_PRINTF,"printf",t.lineNumber)));
		//printf_node->addChild(print_node->children[1]);
		vector<Variable> v;
		for (auto x : exprPrint_node->children)
		{
			Pattern p;
			if(auto exprNode = dynamic_pointer_cast<BinaryOpNode>(x)){
				if (exprNode->name == "expressionInPrint")
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%f", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
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
					p = scopesFinal[indexScope][tokNode->token.value].type;

				}
				case TOK_INT:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%d", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				case TOK_STRING:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%s", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				case TOK_CHAR:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%c", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				case TOK_LONG:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%ld", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				case TOK_FLOAT:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%f", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				case TOK_DOUBLE:
				{
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_TYPE_PRINT, "%c", t.lineNumber)));
					printf_node->addChild(make_shared<TokenNode>(Token(TOK_COMMA, ",", t.lineNumber)));
					break;
				}
				default:
					break;
				}
			}
			
		}
		//printf_node->addChild(print_node->children[2]);

		for (auto x : exprPrint_node->children)
		{
			if (auto tokNode = dynamic_pointer_cast<TokenNode>(x))
			{
				if(tokNode->token.typeToken!=TOK_PLUS&& tokNode->token.typeToken != TOK_STRING_LITERAL)
					printf_node->addChild(x);
			}
		}
		print_node->changeChild(printf_node, 2);

	}

	shared_ptr<ASTNode> generate_number();
	shared_ptr<ASTNode> generate_type(); // ניתוח סוגי משתנים

	//משתנים
	shared_ptr<ASTNode> declaration();
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

