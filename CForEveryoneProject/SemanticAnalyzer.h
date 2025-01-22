#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include "ASTNode.h"

using namespace std;


// מחלקת ניתוח סמנטי
class SemanticAnalyzer {
private:
	map<string, int> variableScope; // טווח משתנים
	vector<map<string, int>> scopes; // רשימה של טווחים

public:
	void enterScope() {
		scopes.push_back(variableScope); // כניסה לטווח חדש
	}

	void exitScope() {
		if (!scopes.empty()) {
			variableScope = scopes.back(); // החזרת משתנים מהטווח הקודם
			scopes.pop_back(); // חזרה לטווח הקודם
		}
	}
	void declareVariable(shared_ptr<ParentNode> node)
	{
			auto children = node->children;
			for (size_t i = 0; i < children.size(); i += 2)
			{
				if (auto varNode2 = dynamic_pointer_cast<ParentNode>(children[i])) {
					auto children2 = varNode2->children;
					if (auto varNode3 = dynamic_pointer_cast<TokenNode>(children2[0])) {
						defineVariable(varNode3->token.value);
					}
				}
			}

		
	}
	void defineVariable(const string& name) {
		if (variableScope.find(name) != variableScope.end()) {
			throw runtime_error("Variable " + name + " already defined in this scope");
		}
		variableScope[name] = 0; // הוספת משתנה לטווח
	}



	void useVariable(const string& name)
	{
		try
		{
			if (variableScope.find(name) == variableScope.end()) {
				string s = "Variable " + name + " is used before it is defined\n";
				throw s;
			}
			//variableScope[name] = 0; // הוספת משתנה לטווח
		}
		catch (string msg)
		{
			cout << msg;
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
			case TOK_OPEN_CURLY: { enterScope(); }
							   break;
			case TOK_CLOSE_CURLY: { exitScope(); }
								break;
			default:
				break;
			}

		}
		else if (auto parentNode = dynamic_pointer_cast<ParentNode>(node)) {
			// ניתוח צומת אב רגיל
			if (parentNode->name == "variable_list")
				declareVariable(parentNode);
			for (auto& child : parentNode->children) {
				analyze(child); // ניתוח ילד
			}

		}
	}
};