#pragma once
#include <string>
using namespace std;
class Token
{
public:
	string type;   // סוג הטוקן (Keyword, Identifier, Punctuation וכו')
	string value;
	// הערך של הטוקן (כמו שם המשתנה או המילה שמורה)
	int lineNumber;

	Token(string type, string value,int lineNumber):type(type),value(value),lineNumber(lineNumber){}
};

