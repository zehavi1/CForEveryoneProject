#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Token.h"
#include "enums.h"

using namespace std;


class Lexer {
private:
    const vector<string> multiCharTokens = { "==", "!=", ">=", "<=", "&&", "||" }; // דוגמאות לטוקנים מרובי-תווים
    const vector<char> singleCharTokens = { ';', ',', '{', '}', '(', ')', '+', '-', '*', '/', '=', '<', '>','"','[',']','%'}; // טוקנים חד-תווים
    const std::map<std::string, Pattern> mapSaved = {
    {"var", TOK_VAR},
    {"true", TOK_TRUE},
    {"string", TOK_STRING},
    {"return", TOK_RETURN},
    {"print", TOK_PRINT},
    {"new", TOK_NEW},
    {"long", TOK_LONG},
    {"int", TOK_INT},
    {"ifrange", TOK_IFRANGE},
    {"if", TOK_IF},
    {"for", TOK_FOR},
    {"flout", TOK_FLOUT},
    {"false", TOK_FALSE},
    {"else", TOK_ELSE},
    {"elif", TOK_ELIF},
    {"char", TOK_CHAR},
    {"bool", TOK_BOOL},
    };
    const std::map<std::string, Pattern> mapNonAlphaTokens = {
    {"==", TOK_EQUAL},
    {"!=", TOK_NOT_EQUAL},
    {">=", TOK_GREATER_EQUAL},
    {"<=", TOK_LESS_EQUAL},
    {"&&", TOK_AND},
    {"||", TOK_OR},
    {";", TOK_SEMICOLON},
    {",", TOK_COMMA},
    {"{", TOK_OPEN_CURLY},
    {"}", TOK_CLOSE_CURLY},
    {"(", TOK_OPEN_PAREN},
    {")", TOK_CLOSE_PAREN},
    {"+", TOK_PLUS},
    {"-", TOK_MINUS},
    {"*", TOK_ASTERISK},
    {"/", TOK_SLASH},
    {"=", TOK_ASSIGN},
    {"<", TOK_LESS},
    {">", TOK_GREATER},
    {"\"", TOK_STRING_LITERAL},
    {"[",TOK_LEFT_ARRAY},
    {"]",TOK_RIGHT_ARRAY},
    {"%",TOK_MODULO}
    };
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
    vector<Token> tokenize(const string& input);
   // Token getToken(const string& buffer, const Graph& graph);

    // קריאה של תוכן קובץ למחרוזת אחת
    string readFileToString(const string& filename);

    // הדפסת הטוקנים שזוהו
    void printTokens(const vector<Token>& tokens) const;
};
