#pragma once
#include "SemanticAnalyzer.h"
#include "vector"

class CodeGenerator
{
	shared_ptr<ASTNode> ast;
	map<string, Variable> currentScope;
	vector<Token> tokens;
	shared_ptr<ParentNode> freeAll;
	int indexScope = 0;
public:
	CodeGenerator(shared_ptr<ASTNode>& ast) :ast(ast) {};
	CodeGenerator(shared_ptr<ASTNode>& ast, vector<Token>& tokens) :ast(ast),tokens(tokens) {};
	void CodeGenerator_main() {
		addDefineBoolAndIncludes();
		generateCode(ast);
		//addFree();
		
	}
	shared_ptr<ASTNode>& getNewAst() { return ast; }
	void generateCode(shared_ptr<ASTNode>& node);
	void generate_elif(shared_ptr<ParentNode>& node);
	void generate_foreach_loop(shared_ptr<ParentNode>& node);
	shared_ptr<ParentNode>& insertAfter(shared_ptr<ASTNode>& node, shared_ptr<ASTNode>& nodeToInsert);
	void generate_ifrange(shared_ptr<BinaryOpNode>& node);
	void generate_print_statement(shared_ptr<ParentNode>& node);
	void generate_dynamic_array(shared_ptr<ParentNode>& node);
	void addDefineBoolAndIncludes();
	void addFree();
	void addFree(shared_ptr<ParentNode>& node);
	void addAllocAfterArrayDeclaration(shared_ptr<ParentNode>& node, map<string, Variable> variables);

	//משתנים
	void generate_declaration(shared_ptr<ParentNode>& node);
	
};

