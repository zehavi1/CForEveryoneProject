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
	void generate_dynamic_array(shared_ptr<ParentNode>& node);
	void addDefineBoolAndIncludes();
	//משתנים
	void generate_declaration(shared_ptr<ParentNode>& node);
	
};

