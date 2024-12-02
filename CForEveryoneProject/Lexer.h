#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "Graph.h"

using namespace std;

class Lexer {
private:
    const vector<string> multiCharTokens = { "==", "!=", ">=", "<=", "&&", "||" }; // דוגמאות לטוקנים מרובי-תווים
    const vector<char> singleCharTokens = { ';', ',', '{', '}', '(', ')', '+', '-', '*', '/', '=', '<', '>','"'}; // טוקנים חד-תווים

    bool isIdentifierStart(char c) const {
        return isalpha(static_cast<unsigned char>(c)) || c == '_';
    }

    bool isIdentifierPart(char c) const {
        return isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    bool isDigit(char c) const {
        return isdigit(static_cast<unsigned char>(c));
    }

    bool isWhitespace1(char c) const {
        return isspace(static_cast<unsigned char>(c));
    }
    bool isWhitespace(char c) const {
        return c==' ';
    }
    // פונקציה לבדיקת טוקן מרובה-תווים
    bool isMultiCharToken(const string& str) const {
        return find(multiCharTokens.begin(), multiCharTokens.end(), str) != multiCharTokens.end();
    }

public:
    // הפונקציה המרכזית לניתוח לקסיקלי
    vector<Token> tokenize(const string& input, const Graph& graph);

    // קריאה של תוכן קובץ למחרוזת אחת
    string readFileToString(const string& filename);

    // הדפסת הטוקנים שזוהו
    void printTokens(const vector<Token>& tokens) const;
};
