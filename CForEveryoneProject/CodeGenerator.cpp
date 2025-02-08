#include "CodeGenerator.h"
void CodeGenerator:: generateCode(shared_ptr<ASTNode>& node) {
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
					if (parentNode->name == "elif")
					{
						generate_elif(parentNode);
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
				if (binOpNode->name == "range")
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
void CodeGenerator::generate_elif(shared_ptr<ParentNode>& node)
{
	shared_ptr<TokenNode> elseToken = make_shared<TokenNode>(Token(TOK_ELSE, "else", 1));
	shared_ptr<TokenNode> ifToken = make_shared<TokenNode>(Token(TOK_IF, "if", 1));
	shared_ptr<ParentNode> elseifNode = make_shared<ParentNode>("else-if");
	elseifNode->addChild(elseToken);
	elseifNode->addChild(ifToken);
	node->changeChild(elseifNode, 0);
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
	shared_ptr<BinaryOpNode> new_node_second = make_shared<BinaryOpNode>("less", op, exp, second);
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


//משתנים
//in the middle---
void CodeGenerator::generate_declaration(shared_ptr<ParentNode>& node)
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
