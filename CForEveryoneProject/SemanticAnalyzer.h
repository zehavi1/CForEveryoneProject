#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include "ASTNode.h"
#include "Variable.h"
#include "stack"
#include "Function.h"



// מחלקת ניתוח סמנטי
class SemanticAnalyzer
{
private:
	map<string, Variable> variableScope;
	//vector<map<string, Variable>> scopes;
	stack<map<string, Variable>> scopes;
	map<string, Function> functions;
	//static vector<map<string, Variable>> scopesFinal;


public:
	friend class codeGenerator;
	void enterScope() {
		if (!scopes.empty())
			scopes.top() = variableScope;
		scopes.push(variableScope); // כניסה לטווח חדש
	}
	map<string, Function>& getFunctions() { return functions; }
	void exitScope(shared_ptr<ParentNode> node) {
		if (!scopes.empty()) {
			node->variableScope = variableScope;
			//scopesFinal.push_back(scopes.back());
			scopes.pop(); // חזרה לטווח הקודם
			variableScope = scopes.empty() ? map<string, Variable>() : scopes.top(); // החזרת משתנים מהטווח הקודם
		}
	}
	void declareVariable(shared_ptr<ParentNode> node)
	{
		//auto children = node->children;
		shared_ptr<ParentNode> list;
		shared_ptr<TokenNode> typeVar;
		const std::map<Pattern, Pattern> mapTypes1 = {
{TOK_BOOL, TOK_BOOL_TYPE},
{TOK_CHAR, TOK_CHAR_TYPE},
{TOK_DOUBLE, TOK_DOUBLE_TYPE},
{TOK_FLOAT, TOK_FLOAT_TYPE},
{TOK_INT, TOK_INT_TYPE},
{TOK_LONG, TOK_LONG_TYPE},
{TOK_STRING, TOK_STRING_TYPE} };
		typeVar = dynamic_pointer_cast<TokenNode>(node->children[0]);
		list = dynamic_pointer_cast<ParentNode>(node->children[1]);
		if (typeVar->token.typeToken == TOK_VAR)
		{
			auto varNode2 = dynamic_pointer_cast<ParentNode>(list->children[0]);
			auto varNode3 = dynamic_pointer_cast<TokenNode>(varNode2->children[2]);
			Pattern typeVar2 = varNode3->token.typeToken;
			Pattern typeVar3;
			if (typeVar2 == TOK_ID)
			{
				typeVar2 = variableScope[varNode3->token.value].token.typeToken;

			}
			else {
				typeVar2 = mapTypes1.at(typeVar2);
			}
			for (size_t i = 0; i < list->children.size(); i += 2)
			{
				if (auto varNode2 = dynamic_pointer_cast<ParentNode>(list->children[i])) {
					auto children2 = varNode2->children;
					if (auto varNode3 = dynamic_pointer_cast<TokenNode>(children2[0])) {
						auto varNode4 = dynamic_pointer_cast<TokenNode>(children2[2]);
						typeVar3 = variableScope[varNode4->token.value].token.typeToken;

						varNode3->token.typeToken = typeVar2;
						defineVariable(varNode3->token);
					}
				}

			}
		}
		else {
			for (size_t i = 0; i < list->children.size(); i += 2)
			{
				if (auto varNode2 = dynamic_pointer_cast<ParentNode>(list->children[i])) {
					auto children2 = varNode2->children;
					shared_ptr<TokenNode> varNode3;
					i = 0;
					do {
						 varNode3 = dynamic_pointer_cast<TokenNode>(children2[i]);
						i++;
					} while (!varNode3 || varNode3->token.typeToken != TOK_ID);
					varNode3->token.typeToken = typeVar->token.typeToken;
					defineVariable(varNode3->token);
				}

			}
		}

	}
	void declareVariableInForeach(shared_ptr<ParentNode> node)
	{
		//auto children = node->children;
		shared_ptr<TokenNode> var;
		shared_ptr<TokenNode> typeVar;

		typeVar = dynamic_pointer_cast<TokenNode>(node->children[2]);
		if (typeVar->token.typeToken != TOK_ID)
		{
			var = dynamic_pointer_cast<TokenNode>(node->children[3]);
			var->token.typeToken = typeVar->token.typeToken;
			defineVariable(var->token);
		}
		compareTypesInForeach(node, typeVar);
	}
	bool compareTypesInForeach(shared_ptr<ParentNode> node, shared_ptr<TokenNode> typeVar)
	{
		//   יש לבדוק מהו הסוג של המערך
		auto collectionNode = node->children[3];
		Pattern collectionType;

		// השגת סוג המערך
		if (auto idNode = dynamic_pointer_cast<TokenNode>(collectionNode)) {
			// אם המערך הוא משתנה
			string arrayName = idNode->token.value;
			collectionType = variableScope[arrayName].token.typeToken; // קבלת סוג המערך
		}
		else if (auto arrayNode = dynamic_pointer_cast<ParentNode>(collectionNode)) {
			// אם המערך הוא מערך מורכב
			collectionType = analyzeArrayType(arrayNode); // נניח שיש פונקציה לניתוח סוג המערך
		}

		// בדוק אם הסוג של המשתנה תואם לסוג של המערך
		auto declaredVarType = typeVar->token.typeToken; // קבלת סוג המשתנה שהוגדר
		if (declaredVarType != collectionType) {
			string s = "Type mismatch: variable type" + tokenNames[declaredVarType] + "does not match the collection type" + tokenNames[collectionType];
			throw s;
		}
	}
	Pattern analyzeArrayType(shared_ptr<ParentNode> arrayNode) {

		shared_ptr<TokenNode> p = dynamic_pointer_cast<TokenNode>(arrayNode->children[1]);
		Pattern key = p->token.typeToken; // קח את סוג הפטרן מתוך TokenNode
		Pattern p1 = TOK_ERROR;
		// בדוק אם המפתח קיים במפה
		auto it = mapTypes.find(key);
		if (it != mapTypes.end())
			p1 = it->second; // קבל את הערך המתאים מהמפה
		return p1; // החזרת סוג 
	}
	//void defineVariable1(const string& name) {
	//	if (variableScope.find(name) != variableScope.end()) {
	//		throw runtime_error("Variable " + name + " already defined in this scope");
	//	}
	//	//variableScope[name] = 0; // הוספת משתנה לטווח
	//}
	void defineVariable(Token& token)
	{
		if (variableScope.find(token.value) != variableScope.end()) {
			string s = "Variable " + token.value + " already defined in this scope";
			throw s;
		}
		variableScope[token.value] = Variable(token);

	}


	void useVariable(const string& name)
	{
		auto varScope = variableScope.find(name);
		auto func = functions.find(name);
		if (varScope == variableScope.end()&&func==functions.end()) {
			string s = "Variable " + name + " is used before it is defined\n";
			throw s;
		}
	}
	void analyzeStringDeclaration(shared_ptr<ParentNode> node) {
		Pattern type = dynamic_pointer_cast<TokenNode>(node->children[0])->token.typeToken;
		string name = dynamic_pointer_cast<TokenNode>(node->children[1])->token.value;
		Token t(type, name, 1);
		defineVariable(t);
	}
	void analyzeFunction(shared_ptr<ParentNode> node)
	{
		Pattern typeReturn = dynamic_pointer_cast<TokenNode>(node->children[0])->token.typeToken;
		string nameFunction = dynamic_pointer_cast<TokenNode>(node->children[1])->token.value;
		Function func(nameFunction,typeReturn);
		auto it = std::find_if(node->children.begin()+2, node->children.end(), [](auto child) {
			return child->nodeType == PARAMETER_LIST; 
			});
		auto params = dynamic_pointer_cast<ParentNode>(*it);
		for (size_t i = 0; i < params->children.size(); i+=3)
		{
			auto typeVar = dynamic_pointer_cast<TokenNode>(params->children[i])->token.typeToken;
			auto nameVar = dynamic_pointer_cast<TokenNode>(params->children[i+1])->token.value;
			Token t(typeVar, nameVar, 1);
			defineVariable(t);
			Variable v(t);
			func.addParameter(v);
		}
		functions[nameFunction]= func;
		auto block = node->children.back();
		analyze(block);
	}
	void analyze(shared_ptr<ASTNode> node) 
	{
		try 
		{
			if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
				switch (varNode->token.typeToken)
				{
				case TOK_ID: {
					useVariable(varNode->token.value);
				}
						   break;
				default:
					break;
				}
			}
			else if (auto parentNode = dynamic_pointer_cast<ParentNode>(node))
			{
				switch (parentNode->nodeType)
				{
				case DECLARATION:
				{
					declareVariable(parentNode);
					break;
				}
				case STRING_DECLARATION:
					analyzeStringDeclaration(parentNode);
					break;
				case FOREACH_LOOP:
					declareVariableInForeach(parentNode);
				case FOR_LOOP:
				case WHILE_LOOP:
				{
					enterScope();
					int count = 0;
					for (auto& child : parentNode->children) {
						if (count < 5)
							count++;
						else
							analyze(child); // ניתוח ילד
					}
					exitScope(parentNode);
					break;
				}
				case FUNCTION_DEFINITION:
				{
					enterScope();
					analyzeFunction(parentNode);
					exitScope(parentNode);
					break;
				}
				case BLOCK:
				{
					enterScope();
					for (auto& child : parentNode->children) {
						analyze(child); // ניתוח ילד
					}
					exitScope(parentNode);
					break;
				}
				default:
					for (auto& child : parentNode->children) {
						analyze(child); // ניתוח ילד
					}
					break;
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
	void analyze1(shared_ptr<ASTNode> node) {
		try {
			if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
				switch (varNode->token.typeToken)
				{
				case TOK_ID: {
					useVariable(varNode->token.value);
				}
						   break;
				default:
					break;
				}

			}
			else if (auto parentNode = dynamic_pointer_cast<ParentNode>(node))
			{
				// ניתוח צומת אב רגיל
				if (parentNode->name == "declaration")
					declareVariable(parentNode);
				else
					if (parentNode->name == "for" || parentNode->name == "foreach" || parentNode->name == "while")
					{
						enterScope();
						if (parentNode->name == "foreach")
							declareVariableInForeach(parentNode);
						int count = 0;
						for (auto& child : parentNode->children) {
							if (count < 5)
								count++;
							else
								analyze(child); // ניתוח ילד
						}
						//exitScope();
						exitScope(parentNode);
					}
					else
						if (parentNode->name == "function")
						{
							enterScope();
							for (auto& child : parentNode->children) {
								analyze(child); // ניתוח ילד
							}
							exitScope(parentNode);
						}
						else
                		if (parentNode->name == "parameters")
								analyze(parentNode);
							else if (parentNode->name == "block")
							{
								enterScope();
								for (auto& child : parentNode->children) {
									analyze(child); // ניתוח ילד
								}
								exitScope(parentNode);
							}
							else
								for (auto& child : parentNode->children) {
									analyze(child); // ניתוח ילד
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


};