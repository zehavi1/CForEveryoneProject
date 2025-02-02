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
		Variable() {};
		Variable(Token& token, bool initialized = false)
			: token(token), initialized(initialized) {
		}
};

