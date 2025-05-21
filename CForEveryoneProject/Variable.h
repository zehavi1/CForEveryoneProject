#pragma once
#include <string>
#include "enums.h"
#include "Token.h"
using namespace std;
class Variable
{
public:
	Token token;
	bool initialized;
	bool isArray;
	Variable() {};
	Variable(Token& token, bool isArray = false, bool initialized = false)
		: token(token), isArray(isArray), initialized(initialized) {
	}
};

