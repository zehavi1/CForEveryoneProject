#include <iostream>
#include "graph.h"

int main() {
    Graph g;
    g.loadFromCSV("‏‏automations1.csv");  // נתיב הקובץ שלך

    // הדפסת הגרף
    g.printGraph();

    // הצגת כל המסלולים למצב טוקן ממצב התחלתי מסוים
    g.printPathsToToken("f");  // התחל ממצב התחלה שאתה רוצה

    return 0;
}
