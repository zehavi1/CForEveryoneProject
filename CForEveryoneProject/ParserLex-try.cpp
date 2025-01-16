//#include <vector>
//#include <stdexcept>
//#include "Token.h"
//#include "ASTNode.h"
//using namespace std;
//class ParserLex {
//    vector<Token> tokens; // רשימת הטוקנים
//    size_t currentTokenIndex = 0;
//    // האינדקס של הטוקן הנוכחי
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
//        if (currentTokenIndex < tokens.size() - 1) {
//            currentTokenIndex++;
//        }
//    }
//
//    // התאמת טוקן מסוים
//    shared_ptr<TokenNode> match(Pattern pattern,string msg="UnExcepted") {
//		if (currentToken().typeToken != pattern) {
//            string s = currentToken().value + msg;
//            throw s;
//		}
//		Token t = currentToken();
//        nextToken();
//        return make_shared<TokenNode>(t); // ליצור ולהחזיר צומת חדש
//    }
//
//    // ניתוח ביטוי
//    shared_ptr<ASTNode> expression() {
//        shared_ptr<ASTNode> left = term();
//
//        while (currentToken().typeToken == TOK_PLUS || currentToken().typeToken == TOK_MINUS) {
//            string op = tokenNames[currentToken().typeToken];
//            nextToken();
//            shared_ptr<ASTNode> right = term();
//            left = make_shared<BinaryOpNode>(op, left, right);
//        }
//
//        return left;
//    }
//
//    // ניתוח טרמינל
//    shared_ptr<ASTNode> term() {
//        shared_ptr<ASTNode> left = factor();
//
//        while (currentToken().typeToken == TOK_ASTERISK || currentToken().typeToken == TOK_SLASH||currentToken().typeToken==TOK_PERCENT) {
//            string op = tokenNames[currentToken().typeToken];
//            nextToken();
//            shared_ptr<ASTNode> right = factor();
//            left = make_shared<BinaryOpNode>(op, left, right);
//        }
//
//        return left;
//    }
//
//    // ניתוח גורם (factor)
//    shared_ptr<ASTNode> factor() {
//		shared_ptr<ParentNode> factorNode = make_shared<ParentNode>("factor");
//        // מאחסן את סוג הטוקן הנוכחי
//        Pattern pattern = currentToken().typeToken;
//
//        switch (pattern) {
//        case TOK_ID: // במקרה שהטוקן הוא מזהה
//        case TOK_BOOL: // במקרה שהטוקן הוא Boolean
//        case TOK_CHAR: // במקרה שהטוקן הוא תו
//        case TOK_DOUBLE: // במקרה שהטוקן הוא מספר כפול
//        case TOK_INT: // במקרה שהטוקן הוא מספר שלם
//		case TOK_FLOAT: // במקרה שהטוקן הוא מספר עשרוני
//		case TOK_LONG:
//        {
//            // במקרה שהטוקן הוא מספר שלם ארוך
//            shared_ptr<ASTNode> node = number(); 
//            // ניתוח מספר
//			return node; // החזרת המספר
//			//Token t = currentToken();
//            //nextToken(); // לעבור לטוקן הבא
//            //return make_shared<TokenNode>(t); // ליצור ולהחזיר טון חדש
//            // אין צורך ב-break כאן כי return יוצא מהפונקציה
//        }
//        case TOK_OPEN_PAREN: // במקרה של סוגריים פתוחים
//        {
//			
//            factorNode->addChild(match(TOK_OPEN_PAREN)); // ליצור ולהחזיר טון חדש
//            factorNode->addChild(expression()); // ניתוח הביטוי שבתוך הסוגריים
//            factorNode->addChild(match(TOK_CLOSE_PAREN,"Expected closing parenthesis"));  // throw runtime_error("Expected closing parenthesis"); // זריקת חריגה אם אין סוגריים סגורים
//            return factorNode; // החזרת הביטוי 
//        }
//        break;
//		case TOK_INCREMENT: // במקרה של אופרטור הגדלה
//		case TOK_DECREMENT: // במקרה של אופרטור הפחתה
//        {
//			Token t = currentToken();
//            nextToken(); // לעבור לטוקן הבא
//            return make_shared<TokenNode>(t); // ליצור ולהחזיר טון חדש
//        }
//        default: // במקרה של טוקן לא צפוי
//            match(TOK_EOF,"Unexpected token in parseFactor"); // זריקת חריגה עם הודעה מתאימה
//        }
//    }
//	shared_ptr<ASTNode> number() {
//        Pattern p = currentToken().typeToken;
//		if (p == TOK_INT || p == TOK_DOUBLE || p == TOK_FLOAT || p == TOK_LONG||p==TOK_CHAR) {
//			Token t = currentToken();
//			nextToken();
//			return make_shared<TokenNode>(t);
//		}
//		else {
//			match(TOK_EOF,"Expected number");
//		}
//	}
//    shared_ptr<ASTNode> print_statement()
//    {
//		shared_ptr<ParentNode> printNode = make_shared<ParentNode>("print");
//		printNode->addChild(match(TOK_PRINT)); // להתאים את הטוקן הנוכחי לפונקציה
//		printNode->addChild(match(TOK_OPEN_PAREN, "Expected '(' after print")); // להתאים את הטוקן הנוכחי לפונקציה
//		printNode->addChild(expression()); // להתאים את הטוקן הנוכחי לפונקציה
//		printNode->addChild(match(TOK_CLOSE_PAREN, "Expected ')' after expression")); // להתאים את הטוקן הנוכחי לפונקציה
//		printNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement")); // להתאים את הטוקן הנוכחי לפונקציה
//            return printNode; // נניח שבינתיים משפט הוא ביטוי
//        
//    }
//
//	
//	// ניתוח השמה-assignment--לא לשכוח לעשות גם לאתחול משתנים
//	shared_ptr<ASTNode> assignment() {
//		shared_ptr<ParentNode> startNode = make_shared<ParentNode>("assignment");
//		startNode->addChild(match(TOK_ID, "Expected identifier"));
//		startNode->addChild(match(TOK_EQUAL,"Expected '=' after identifier"));
//		startNode->addChild(expression());
//		startNode->addChild(match(TOK_SEMICOLON, "Expected ';' at the end of statement"));
//		return startNode;
//	}
//
//    // ניתוח משפט
//    shared_ptr<ASTNode> parseStatement() {
//        
//		Pattern p = currentToken().typeToken;
//        switch (p)
//        {
//        case TOK_DOUBLE_TYPE:
//            break;
//        case TOK_INT_TYPE:
//            break;
//        case TOK_CHAR_TYPE:
//            break;
//        case TOK_BOOL_TYPE:
//            break;
//        case TOK_FLOAT_TYPE:
//            break;
//        case TOK_STRING_TYPE:
//            break;
//        case TOK_LONG_TYPE:
//            break;
//        case TOK_VOID:
//            break;
//        case TOK_INCLUDE:
//            break;
//        /*case TOK_DOUBLE:
//            break;*/
//        case TOK_VAR:
//            break;
//        /*case TOK_TRUE:
//            break;*/
//        case TOK_RETURN:
//            break;
//        case TOK_PRINT: { return print_statement(); }
//            break;
//        /*case TOK_NEW:
//            break;
//        case TOK_LONG:
//            break;
//        case TOK_INT:
//            break;*/
//        case TOK_IFRANGE:
//            break;
//        case TOK_IF:
//            break;
//        case TOK_FOR:
//            break;
//        case TOK_FLOAT:
//            break;
//        /*case TOK_FALSE:
//            break;
//        case TOK_ELSE:
//            break;
//        case TOK_ELIF:
//            break;
//        case TOK_CHAR:
//            break;
//        case TOK_BOOL:
//            break;
//        case TOK_ERROR:
//            break;*/
//        case TOK_ID:
//            break;
//        /*case TOK_STRING:
//            break;
//        case TOK_EQUAL:
//            break;
//        case TOK_NOT_EQUAL:
//            break;
//        case TOK_GREATER_EQUAL:
//            break;
//        case TOK_LESS_EQUAL:
//            break;
//        case TOK_AND:
//            break;
//        case TOK_OR:
//            break;
//        case TOK_SEMICOLON:
//            break;*/
//        case TOK_COMMA:
//            break;
//        case TOK_OPEN_CURLY:
//            break;
//        case TOK_CLOSE_CURLY:
//            break;
//        case TOK_OPEN_PAREN:
//            break;
//        case TOK_CLOSE_PAREN:
//            break;
//        case TOK_PLUS:
//            break;
//        case TOK_MINUS:
//            break;
//        case TOK_ASTERISK:
//            break;
//        case TOK_SLASH:
//            break;
//        case TOK_ASSIGN:
//            break;
//        case TOK_LESS:
//            break;
//        case TOK_GREATER:
//            break;
//        case TOK_STRING_LITERAL:
//            break;
//        case TOK_RIGHT_ARRAY:
//            break;
//        case TOK_LEFT_ARRAY:
//            break;
//        case TOK_PERCENT:
//            break;
//        case TOK_EOF:
//            break;
//        case TOK_UNKNOWN:
//            break;
//        case TOK_INCREMENT:
//            break;
//        case TOK_DECREMENT:
//            break;
//        default:
//            break;
//        }
//         if (match(TOK_INT)) {
//            if (!match(TOK_ID)) {
//                throw runtime_error("Expected identifier after int");
//            }
//            if (!match(TOK_ASSIGN)) {
//                throw runtime_error("Expected '=' after identifier");
//            }
//            shared_ptr<ASTNode> expr = expression();
//            if (!match(TOK_SEMICOLON)) {
//                throw runtime_error("Expected ';' at the end of statement");
//            }
//            return expr;
//        }
//        else {
//            throw runtime_error("Unknown statement type");
//        }
//    }
//
//public:
//    ParserLex(vector<Token> tokens) : tokens(tokens) {}
//
//    // ניתוח הקלט כולו
//    vector<shared_ptr<ASTNode>> parse() {
//        vector<shared_ptr<ASTNode>> ast;
//        try {
//            while (currentTokenIndex < tokens.size() - 1) {
//                ast.push_back(parseStatement());
//            }
//            return ast;
//        }
//		catch (string s) {
//			cout << s << endl;
//		}
//        
//    }
//   
//    void printAST(const shared_ptr<ASTNode>& node, int depth = 0) {
//        if (!node) return;
//
//        // הדפסת רווחים בהתאם לעומק העץ
//        for (int i = 0; i < depth; ++i) {
//            cout << "  ";
//        }
//
//        // הדפסת סוג הצומת
//        if (auto numNode = dynamic_pointer_cast<TokenNode>(node)) {
//            cout << "tokenNode: " << numNode->token.value << endl;
//        }
//        
//        else if (auto binOpNode = dynamic_pointer_cast<BinaryOpNode>(node)) {
//            cout << "BinaryOpNode: " << binOpNode->op << endl;
//            printAST(binOpNode->left, depth + 1);  // צד שמאל
//            printAST(binOpNode->right, depth + 1); // צד ימין
//        }
//        
//        else if (auto stmtNode = dynamic_pointer_cast<ParentNode>(node)) {
//            cout << "parentNode of"<<stmtNode->name <<":"<< endl;
//			for (const auto& child : stmtNode->children)
//				printAST(child, depth + 1);
//        }
//        else {
//            cout << "Unknown Node" << endl;
//        }
//    }
//
//};
//
//
