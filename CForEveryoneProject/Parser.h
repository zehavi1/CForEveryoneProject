//#pragma once
//#include <vector>
//#include <stdexcept>
//#include "Token.h"
//#include "ASTNode.h"
//using namespace std;
//class Parser {
//    vector<Token> tokens; // רשימת הטוקנים
//    size_t currentTokenIndex = 0;
//	// האינדקס של הטוקן הנוכחי
//    // הפונקציה שמחזירה את הטוקן הנוכחי
//    Token currentToken() {
//        if (currentTokenIndex >= tokens.size()) {
//            throw runtime_error("Unexpected end of input");
//        }
//        return tokens[currentTokenIndex];
//    }
//
//    // התקדמות לטוקן הבא
//    void nextToken() {
//        if (currentTokenIndex < tokens.size()-1) {
//            currentTokenIndex++;
//        }
//    }
//
//    // התאמת טוקן מסוים
//    bool match(Pattern pattern) {
//        if (currentToken().typeToken == pattern) {
//            nextToken();
//            return true;
//        }
//        return false;
//    }
//
//    // ניתוח ביטוי
//    shared_ptr<ASTNode> parseExpression() {
//        shared_ptr<ASTNode> left = parseTerm();
//
//        while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
//            string op = tokenNames[currentToken().typeToken];
//            nextToken();
//            shared_ptr<ASTNode> right = parseTerm();
//            left = make_shared<BinaryOpNode>(op, left, right);
//        }
//
//        return left;
//    }
//
//    // ניתוח טרמינל
//    shared_ptr<ASTNode> parseTerm() {
//        shared_ptr<ASTNode> left = parseFactor();
//
//        while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH) {
//            string op = tokenNames[currentToken().typeToken];
//            nextToken();
//            shared_ptr<ASTNode> right = parseFactor();
//            left = make_shared<BinaryOpNode>(op, left, right);
//        }
//
//        return left;
//    }
//
//    // ניתוח גורם (factor)
//    shared_ptr<ASTNode> parseFactor() {
//        if (currentToken().typeToken == TOK_INT) {
//            double value = stod(currentToken().value);
//            nextToken();
//            return make_shared<NumberNode>(value);
//        }
//        else if (currentToken().typeToken == TOK_ID) {
//            string name = currentToken().value;
//            nextToken();
//            return make_shared<NumberNode>(name);
//        }
//        else if (match(TOK_OPEN_PAREN)) {
//            shared_ptr<ASTNode> expr = parseExpression();
//            if (!match(TOK_CLOSE_PAREN)) {
//                throw runtime_error("Expected closing parenthesis");
//            }
//            return expr;
//        }
//        else {
//            throw runtime_error("Unexpected token in parseFactor");
//        }
//    }
//
//    // ניתוח משפט
//    shared_ptr<ASTNode> parseStatement() {
//        if (match(TOK_PRINT)) {
//            if (!match(TOK_OPEN_PAREN)) {
//                throw runtime_error("Expected '(' after print");
//            }
//            shared_ptr<ASTNode> expr = parseExpression();
//            if (!match(TOK_CLOSE_PAREN)) {
//                throw runtime_error("Expected ')' after expression");
//            }
//            if (!match(TOK_SEMICOLON)) {
//                throw runtime_error("Expected ';' at the end of statement");
//            }
//            return expr; // נניח שבינתיים משפט הוא ביטוי
//		}
//		else if (match(TOK_INT)) {
//			if (!match(TOK_ID)) {
//				throw runtime_error("Expected identifier after int");
//			}
//			if (!match(TOK_ASSIGN)) {
//				throw runtime_error("Expected '=' after identifier");
//			}
//			shared_ptr<ASTNode> expr = parseExpression();
//			if (!match(TOK_SEMICOLON)) {
//				throw runtime_error("Expected ';' at the end of statement");
//			}
//			return make_shared<StatementNode>(expr);
//		}
//        else {
//            throw runtime_error("Unknown statement type");
//        }
//    }
//
//public:
//    Parser(vector<Token> tokens) : tokens(tokens) {}
//
//    // ניתוח הקלט כולו
//    vector<shared_ptr<ASTNode>> parse() {
//        vector<shared_ptr<ASTNode>> ast;
//        while (currentTokenIndex < tokens.size()-1) {
//            ast.push_back(parseStatement());
//        }
//        return ast;
//    }
//    //void printAST(const shared_ptr<ASTNode>& node, int depth = 0, bool isLast = true) {
//    //    if (!node) return;
//
//    //    // הדפסת רווחים והתאמת תצוגה גרפית לפי העומק
//    //    for (int i = 0; i < depth; ++i) {
//    //        if (i == depth - 1 && !isLast) {
//    //            cout << "|   ";
//    //        }
//    //        else if (i < depth - 1) {
//    //            cout << "    ";
//    //        }
//    //    }
//
//    //    // סמלים גרפיים
//    //    if (depth > 0) {
//    //        cout << (isLast ? "- " : "|- ");
//    //    }
//
//    //    // זיהוי סוג הצומת
//    //    if (auto numNode = dynamic_pointer_cast<NumberNode>(node)) {
//    //        cout << "TOK_INT(" << numNode->value << ")" << endl;
//    //    }
//    //    else if (auto idNode = dynamic_pointer_cast<IdentifierNode>(node)) {
//    //        cout << "TOK_ID(" << idNode->name << ")" << endl;
//    //    }
//    //    else if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node)) {
//    //        cout << "<BinaryOpNode> " << binOpNode->op << endl;
//    //        printAST(binOpNode->left, depth + 1, false);  // צד שמאל
//    //        printAST(binOpNode->right, depth + 1, true); // צד ימין
//    //    }
//    //    else if (auto exprNode = dynamic_pointer_cast<ExpressionNode>(node)) {
//    //        cout << "<ExpressionNode>" << endl;
//    //        printAST(exprNode->expression, depth + 1, true);
//    //    }
//    //    else if (auto stmtNode = dynamic_pointer_cast<StatementNode>(node)) {
//    //        cout << "<StatementNode>" << endl;
//    //        printAST(stmtNode->statement, depth + 1, true);
//    //    }
//    //    else {
//    //        cout << "<Unknown Node>" << endl;
//    //    }
//    //}
//
//	/*void printAST(const vector<shared_ptr<ASTNode>>& ast) {
//		for (const auto& node : ast) {
//			node->printASTNode();
//		}
//	}*/
//    void printAST(const shared_ptr<ASTNode>& node, int depth = 0) {
//        if (!node) return;
//
//        // הדפסת רווחים בהתאם לעומק העץ
//        for (int i = 0; i < depth; ++i) {
//            cout << "  ";
//        }
//
//        // הדפסת סוג הצומת
//        if (auto numNode = dynamic_pointer_cast<NumberNode>(node)) {
//            cout << "NumberNode: " << numNode->value << endl;
//        }
//        else if (auto idNode = dynamic_pointer_cast<TokenNode>(node)) {
//            cout << "IdentifierNode: " << idNode->token.typeToken << endl;
//        }
//        else if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node)) {
//            cout << "BinaryOpNode: " << binOpNode->op << endl;
//            printAST(binOpNode->left, depth + 1);  // צד שמאל
//            printAST(binOpNode->right, depth + 1); // צד ימין
//        }
//        else if (auto exprNode = dynamic_pointer_cast<ExpressionNode>(node)) {
//            cout << "ExpressionNode:" << endl;
//            printAST(exprNode->expression, depth + 1);
//        }
//        else if (auto stmtNode = dynamic_pointer_cast<StatementNode>(node)) {
//            cout << "StatementNode:" << endl;
//            printAST(stmtNode->statement, depth + 1);
//        }
//        else {
//            cout << "Unknown Node" << endl;
//        }
//    }
//
//};
//
//
