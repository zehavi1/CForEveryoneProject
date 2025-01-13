#include <vector>
#include <stdexcept>
#include "Token.h"
#include "ASTNode.h"
using namespace std;
class ParserLex {
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
        if (currentTokenIndex < tokens.size() - 1) {
            currentTokenIndex++;
        }
    }

    // התאמת טוקן מסוים
    shared_ptr<TokenNode> match(Pattern pattern,string msg="UnExcepted") {
		if (currentToken().typeToken != pattern) {
			throw currentToken().value + msg;
		}
        nextToken();
        return make_shared<TokenNode>(currentToken()); // ליצור ולהחזיר צומת חדש
    }

    // ניתוח ביטוי
    shared_ptr<ASTNode> expression() {
        shared_ptr<ASTNode> left = term();

        while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
            string op = tokenNames[currentToken().typeToken];
            nextToken();
            shared_ptr<ASTNode> right = term();
            left = make_shared<BinaryOpNode>(op, left, right);
        }

        return left;
    }

    // ניתוח טרמינל
    shared_ptr<ASTNode> term() {
        shared_ptr<ASTNode> left = factor();

        while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH||currentToken().typeToken==TOK_PERCENT) {
            string op = tokenNames[currentToken().typeToken];
            nextToken();
            shared_ptr<ASTNode> right = factor();
            left = make_shared<BinaryOpNode>(op, left, right);
        }

        return left;
    }

    // ניתוח גורם (factor)
    shared_ptr<ASTNode> factor() {
		shared_ptr<ParentNode> factorNode = make_shared<ParentNode>("factor");
        // מאחסן את סוג הטוקן הנוכחי
        Pattern pattern = currentToken().typeToken;

        switch (pattern) {
        case TOK_ID: // במקרה שהטוקן הוא מזהה
        case TOK_BOOL: // במקרה שהטוקן הוא Boolean
        case TOK_CHAR: // במקרה שהטוקן הוא תו
        case TOK_DOUBLE: // במקרה שהטוקן הוא מספר כפול
        case TOK_INT: // במקרה שהטוקן הוא מספר שלם
		case TOK_FLOAT: // במקרה שהטוקן הוא מספר עשרוני
		case TOK_LONG:
        {
            // במקרה שהטוקן הוא מספר שלם ארוך
            shared_ptr<ASTNode> node = number(); // ניתוח מספר
            nextToken(); // לעבור לטוקן הבא
            return make_shared<TokenNode>(currentToken()); // ליצור ולהחזיר טון חדש
            // אין צורך ב-break כאן כי return יוצא מהפונקציה
        }
        case TOK_OPEN_PAREN: // במקרה של סוגריים פתוחים
        {
			nextToken(); // לעבור לטוקן הבא
            factorNode->addChild(match(TOK_OPEN_PAREN)); // ליצור ולהחזיר טון חדש
            factorNode->addChild(expression()); // ניתוח הביטוי שבתוך הסוגריים
            factorNode->addChild(match(TOK_CLOSE_PAREN,"Expected closing parenthesis"));  // throw runtime_error("Expected closing parenthesis"); // זריקת חריגה אם אין סוגריים סגורים
            return factorNode; // החזרת הביטוי 
        }
        break;
		case TOK_INCREMENT: // במקרה של אופרטור הגדלה
		case TOK_DECREMENT: // במקרה של אופרטור הפחתה
        {
            nextToken(); // לעבור לטוקן הבא
            return make_shared<TokenNode>(currentToken()); // ליצור ולהחזיר טון חדש
        }
        default: // במקרה של טוקן לא צפוי
            throw runtime_error("Unexpected token in parseFactor"); // זריקת חריגה עם הודעה מתאימה
        }
    }
	shared_ptr<ASTNode> number() {
        Pattern p = currentToken().typeToken;
		if (p == TOK_INT || p == TOK_DOUBLE || p == TOK_FLOAT || p == TOK_LONG||p==TOK_CHAR) {
			nextToken();
			return make_shared<TokenNode>(currentToken());
		}
		else {
			throw runtime_error("Expected number");
		}
	}


    
    

    // ניתוח משפט
    shared_ptr<ASTNode> parseStatement() {
        if (match(TOK_PRINT)) {
            if (!match(TOK_OPEN_PAREN)) {
                throw runtime_error("Expected '(' after print");
            }
            shared_ptr<ASTNode> expr = expression();
            if (!match(TOK_CLOSE_PAREN)) {
                throw runtime_error("Expected ')' after expression");
            }
            if (!match(TOK_SEMICOLON)) {
                throw runtime_error("Expected ';' at the end of statement");
            }
            return expr; // נניח שבינתיים משפט הוא ביטוי
        }
        else if (match(TOK_INT)) {
            if (!match(TOK_ID)) {
                throw runtime_error("Expected identifier after int");
            }
            if (!match(TOK_ASSIGN)) {
                throw runtime_error("Expected '=' after identifier");
            }
            shared_ptr<ASTNode> expr = expression();
            if (!match(TOK_SEMICOLON)) {
                throw runtime_error("Expected ';' at the end of statement");
            }
            return make_shared<StatementNode>(expr);
        }
        else {
            throw runtime_error("Unknown statement type");
        }
    }

public:
    ParserLex(vector<Token> tokens) : tokens(tokens) {}

    // ניתוח הקלט כולו
    vector<shared_ptr<ASTNode>> parse() {
        vector<shared_ptr<ASTNode>> ast;
        while (currentTokenIndex < tokens.size() - 1) {
            ast.push_back(parseStatement());
        }
        return ast;
    }
   
    void printAST(const shared_ptr<ASTNode>& node, int depth = 0) {
        if (!node) return;

        // הדפסת רווחים בהתאם לעומק העץ
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }

        // הדפסת סוג הצומת
        if (auto numNode = dynamic_pointer_cast<TokenNode>(node)) {
            cout << "tokenNode: " << numNode->token.value << endl;
        }
        
        else if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node)) {
            cout << "BinaryOpNode: " << binOpNode->op << endl;
            printAST(binOpNode->left, depth + 1);  // צד שמאל
            printAST(binOpNode->right, depth + 1); // צד ימין
        }
        else if (auto exprNode = dynamic_pointer_cast<ExpressionNode>(node)) {
            cout << "ExpressionNode:" << endl;
            printAST(exprNode->expression, depth + 1);
        }
        else if (auto stmtNode = dynamic_pointer_cast<StatementNode>(node)) {
            cout << "StatementNode:" << endl;
            printAST(stmtNode->statement, depth + 1);
        }
        else {
            cout << "Unknown Node" << endl;
        }
    }

};


