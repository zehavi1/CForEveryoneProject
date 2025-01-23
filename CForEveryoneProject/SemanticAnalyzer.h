#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include "ASTNode.h"

using namespace std;
struct Variable {
	string name;
	Pattern type;
	bool initialized;
	Variable() {};
	Variable(const string& name, Pattern type, bool initialized = false)
		: name(name), type(type), initialized(initialized) {
	}
};

// מחלקת ניתוח סמנטי
class SemanticAnalyzer
{
private:
	map<string, Variable> variableScope;
	vector<map<string, Variable>> scopes;
	//map<string, int> variableScope; // טווח משתנים
	//vector<map<string, int>> scopes; // רשימה של טווחים

public:
	void enterScope() {
		if(!scopes.empty())
			scopes.back() = variableScope;
		scopes.push_back(variableScope); // כניסה לטווח חדש
	}

	void exitScope() {
		if (!scopes.empty()) {
			scopes.pop_back(); // חזרה לטווח הקודם
			variableScope = scopes.back(); // החזרת משתנים מהטווח הקודם
		}
	}
	void declareVariable(shared_ptr<ParentNode> node)
	{
		//auto children = node->children;
		shared_ptr<ParentNode> list;
		shared_ptr<TokenNode> typeVar;

		typeVar = dynamic_pointer_cast<TokenNode>(node->children[0]);
		list = dynamic_pointer_cast<ParentNode>(node->children[1]);

		auto children = list->children;
		for (size_t i = 0; i < children.size(); i += 2)
		{
			if (auto varNode2 = dynamic_pointer_cast<ParentNode>(children[i])) {
				auto children2 = varNode2->children;
				if (auto varNode3 = dynamic_pointer_cast<TokenNode>(children2[0])) {
					defineVariable(varNode3->token.value, typeVar->token.typeToken);
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
		if (typeVar->token.typeToken == TOK_ID)
			return;
		var = dynamic_pointer_cast<TokenNode>(node->children[3]);
		defineVariable(var->token.value, typeVar->token.typeToken);
	}
	//void defineVariable1(const string& name) {
	//	if (variableScope.find(name) != variableScope.end()) {
	//		throw runtime_error("Variable " + name + " already defined in this scope");
	//	}
	//	//variableScope[name] = 0; // הוספת משתנה לטווח
	//}
	void defineVariable(const string& name, Pattern type)
	{
		if (variableScope.find(name) != variableScope.end()) {
			string s = "Variable " + name + " already defined in this scope";
			throw s;
		}
		variableScope[name] = Variable(name, type);
		
	}


	void useVariable(const string& name)
	{
		auto varScope = variableScope.find(name);
		if (varScope == variableScope.end()) {
			string s = "Variable " + name + " is used before it is defined\n";
			throw s;
		}
	}

	//void analyze(shared_ptr<ASTNode> node) {
	//    if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
	//        try
	//        {
	//            if (varNode->token.typeToken == TOK_ID && variableScope.find(varNode->token.value) == variableScope.end()) {
	//                throw new string("Variable " + varNode->token.value + " is used before it is defined");
	//            }
	//        }
	//        catch (string msg)
	//        {
	//            cout << msg;
	//        }
	//        
	//    }
	//    else if (auto parentNode = dynamic_pointer_cast<ParentNode>(node)) {
	//        enterScope(); // כניסה לטווח חדש
	//        for (auto& child : parentNode->children) {
	//            analyze(child); // ניתוח ילד
	//        }
	//        exitScope(); // חזרה לטווח הקודם
	//    }
	//}
	void analyze(shared_ptr<ASTNode> node) {
		if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
			switch (varNode->token.typeToken)
			{
			case TOK_ID: {
				useVariable(varNode->token.value);
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
					exitScope();
				}
				else if (parentNode->name == "block")
				{
					enterScope();
					for (auto& child : parentNode->children) {
						analyze(child); // ניתוח ילד
					}
					exitScope();
				}
				else
					for (auto& child : parentNode->children) {
						analyze(child); // ניתוח ילד
					}

		}
	}

};