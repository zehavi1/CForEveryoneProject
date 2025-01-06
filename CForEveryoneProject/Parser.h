#pragma once
#include <vector>
#include <stdexcept>
#include "Token.h"
#include "ASTNode.h"
using namespace std;
class Parser {
    vector<Token> tokens; // רשימת הטוקנים
    size_t currentTokenIndex = 0;
	// האינדקס של הטוקן הנוכחי
    // הפונקציה שמחזירה את הטוקן הנוכחי
    Token currentToken() {
        if (currentTokenIndex >= tokens.size()) {
            throw runtime_error("Unexpected end of input");
        }
        return tokens[currentTokenIndex];
    }

    // התקדמות לטוקן הבא
    void nextToken() {
        if (currentTokenIndex < tokens.size()-1) {
            currentTokenIndex++;
        }
    }

    // התאמת טוקן מסוים
    bool match(Pattern pattern) {
        if (currentToken().typeToken == pattern) {
            nextToken();
            return true;
        }
        return false;
    }

    // ניתוח ביטוי
    shared_ptr<ASTNode> parseExpression() {
        shared_ptr<ASTNode> left = parseTerm();

        while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
            string op = tokenNames[currentToken().typeToken];
            nextToken();
            shared_ptr<ASTNode> right = parseTerm();
            left = make_shared<BinaryOpNode>(op, left, right);
        }

        return left;
    }

    // ניתוח טרמינל
    shared_ptr<ASTNode> parseTerm() {
        shared_ptr<ASTNode> left = parseFactor();

        while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH) {
            string op = tokenNames[currentToken().typeToken];
            nextToken();
            shared_ptr<ASTNode> right = parseFactor();
            left = make_shared<BinaryOpNode>(op, left, right);
        }

        return left;
    }

    // ניתוח גורם (factor)
    shared_ptr<ASTNode> parseFactor() {
        if (currentToken().typeToken == TOK_INT) {
            double value = stod(currentToken().value);
            nextToken();
            return make_shared<NumberNode>(value);
        }
        else if (currentToken().typeToken == TOK_ID) {
            string name = currentToken().value;
            nextToken();
            return make_shared<IdentifierNode>(name);
        }
        else if (match(TOK_OPEN_PAREN)) {
            shared_ptr<ASTNode> expr = parseExpression();
            if (!match(TOK_CLOSE_PAREN)) {
                throw runtime_error("Expected closing parenthesis");
            }
            return expr;
        }
        else {
            throw runtime_error("Unexpected token in parseFactor");
        }
    }

    // ניתוח משפט
    shared_ptr<ASTNode> parseStatement() {
        if (match(TOK_PRINT)) {
            if (!match(TOK_OPEN_PAREN)) {
                throw runtime_error("Expected '(' after print");
            }
            shared_ptr<ASTNode> expr = parseExpression();
            if (!match(TOK_CLOSE_PAREN)) {
                throw runtime_error("Expected ')' after expression");
            }
            if (!match(TOK_SEMICOLON)) {
                throw runtime_error("Expected ';' at the end of statement");
            }
            return expr; // נניח שבינתיים משפט הוא ביטוי
        }
        else {
            throw runtime_error("Unknown statement type");
        }
    }

public:
    Parser(vector<Token> tokens) : tokens(tokens) {}

    // ניתוח הקלט כולו
    vector<shared_ptr<ASTNode>> parse() {
        vector<shared_ptr<ASTNode>> ast;
        while (currentTokenIndex < tokens.size()-1) {
            ast.push_back(parseStatement());
        }
        return ast;
    }
	void printAST(const vector<shared_ptr<ASTNode>>& ast) {
		for (const auto& node : ast) {
			node->printASTNode();
		}
	}
};


