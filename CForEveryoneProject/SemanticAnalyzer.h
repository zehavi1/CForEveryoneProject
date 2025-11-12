#pragma once
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include "ASTNode.h"
#include "Variable.h"
#include "stack"
#include "Function.h"
#include "ErrorHandler.h"



// מחלקת ניתוח סמנטי
class SemanticAnalyzer
{
private:
	map<string, Variable> variableScope;
	stack<map<string, Variable>> scopes;
	map<string, Function> functions;
public:
	friend class codeGenerator;
	map<string, Function>& getFunctions() { return functions; }
	void enterScope();
	void exitScope(shared_ptr<ParentNode> node);
	void declareVariable(shared_ptr<ParentNode> node);
	void declareVariableInForeach(shared_ptr<ParentNode> node);
	bool compareTypesInForeach(shared_ptr<ParentNode> node, shared_ptr<TokenNode> typeVar);
	Pattern analyzeArrayType(shared_ptr<ParentNode> arrayNode);
	void defineVariable(Token& token);
	void useVariable(const Token& token);
	void analyzeStringDeclaration(shared_ptr<ParentNode> node);
	void analyzeFunction(shared_ptr<ParentNode> node);
	void analyze(shared_ptr<ASTNode> node);
	void analyze1(shared_ptr<ASTNode> node);


};