#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "ParserLex-try.cpp"

int main() {
    //Graph g;
   // g.loadFromCSV("‏‏automations1.csv");  // נתיב הקובץ שלך

    // הדפסת הגרף
   // g.printGraph();

    // הצגת כל המסלולים למצב טוקן ממצב התחלתי מסוים
   // g.printPathsToToken("start");  // התחל ממצב התחלה שאתה רוצה
    Lexer lexer;
    //string program = lexer.readFileToString("programExample.txt");
	string program = "print(3 + ++5*2+(a-b));";
    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return 1;
    }
    program += ' ';
    auto tokens = lexer.tokenize(program);
    lexer.printTokens(tokens);
    ParserLex parser(tokens);
    vector<shared_ptr<ASTNode>> ast = parser.parse();
    cout << "Abstract Syntax Tree:" << endl;
    for (const auto& node : ast) {
        parser.printAST(node, 0);
    }
    return 0;
}
