#pragma once
#include <string>
#include "enums.h"
using namespace std;
class Token
{
public:
	Pattern typeToken;   // סוג הטוקן (Keyword, Identifier, Punctuation וכו')
	string value;
	// הערך של הטוקן (כמו שם המשתנה או המילה שמורה)
	int lineNumber;

	Token(Pattern typeToken, string value,int lineNumber=0):typeToken(typeToken),value(value),lineNumber(lineNumber){}
	Token(Pattern typeToken):typeToken(typeToken) {};
	Token() {};
	/*void setType(string type) { this->type = type; }*/
};

