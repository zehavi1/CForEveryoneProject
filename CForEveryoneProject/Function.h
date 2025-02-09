#pragma once
#include <string>
#include <vector>
#include "Variable.h"

using namespace std;
class Function
{
public:
	string name;
	vector<Variable> parameterList;
	Pattern typeReturn=TOK_VOID;
	Function() {}
	Function(string name,Pattern typeReturn) :name(name),typeReturn(typeReturn) {}
	void addParameter(Variable& v) {
		parameterList.push_back(v);
	}
};

