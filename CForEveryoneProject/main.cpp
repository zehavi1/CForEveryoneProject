#include <iostream>
#include "Lexer.h"
#include "SyntacticAnalysis.h"

int main() {
    //Graph g;
   // g.loadFromCSV("‏‏automations1.csv");  // נתיב הקובץ שלך

    // הדפסת הגרף
   // g.printGraph();

    // הצגת כל המסלולים למצב טוקן ממצב התחלתי מסוים
   // g.printPathsToToken("start");  // התחל ממצב התחלה שאתה רוצה
    Lexer lexer;
    //string program = lexer.readFileToString("programExample.txt");
	//string program = "print(3 + ++a*2+(a-b));";
	//string program = "print(3+5);";
    string program = R"(
        int x = 10;
        print(x);
        if (x > 5) {
            print("x is greater than 5");
        }
        return 0;
    )";
    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return 1;
    }
    program += ' ';
    auto tokens = lexer.tokenize(program);
    lexer.printTokens(tokens);
    SyntacticAnalysis parser(tokens);
    vector<shared_ptr<ASTNode>> ast = parser.parse();
    cout << "Abstract Syntax Tree:" << endl;
    for (const auto& node : ast) {
        parser.printASTNodes(node);
    }
    return 0;
}
