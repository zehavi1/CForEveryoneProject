#include <iostream>
#include "Lexer.h"

int main() {
    //Graph g;
   // g.loadFromCSV("‏‏automations1.csv");  // נתיב הקובץ שלך

    // הדפסת הגרף
   // g.printGraph();

    // הצגת כל המסלולים למצב טוקן ממצב התחלתי מסוים
   // g.printPathsToToken("start");  // התחל ממצב התחלה שאתה רוצה
    Lexer lexer;
    string program = lexer.readFileToString("programExample.txt");

    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return 1;
    }

    auto tokens = lexer.tokenize(program);
    lexer.printTokens(tokens);
    return 0;
}
