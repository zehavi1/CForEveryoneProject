#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include "ASTNode.h"
#include "Variable.h"



// מחלקת ניתוח סמנטי
class SemanticAnalyzer
{
private:
	map<string, Variable> variableScope;
	vector<map<string, Variable>> scopes;
	//static vector<map<string, Variable>> scopesFinal;


public:
	friend class codeGenerator;
	void enterScope() {
		if(!scopes.empty())
			scopes.back() = variableScope;
		scopes.push_back(variableScope); // כניסה לטווח חדש
	}

	void exitScope(shared_ptr<ParentNode> node ) {
		if (!scopes.empty()) {
			node->variableScope = variableScope;
			//scopesFinal.push_back(scopes.back());
			scopes.pop_back(); // חזרה לטווח הקודם
			variableScope = scopes.empty() ? map<string, Variable>() : scopes.back(); // החזרת משתנים מהטווח הקודם
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
			defineVariable(var->token);
		}
		compareTypesInForeach(node, typeVar);
	}
	bool compareTypesInForeach(shared_ptr<ParentNode> node,shared_ptr<TokenNode> typeVar)
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
	void defineVariable(Token token)
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
		try {
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
						//exitScope();
						exitScope(parentNode);
					}
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
		catch(string s){
			if (auto varNode = dynamic_pointer_cast<TokenNode>(node)) {
				cout << "line " << varNode->token.lineNumber << ":" << s;
			}
			else
				cout<< s;
		}
		

    }
	

};