#include <iostream>
#include "Lexer.h"
#include "SyntacticAnalysis.h"

int main() {
    //Graph g;
   // g.loadFromCSV("şşautomations1.csv");  // ğúéá ä÷åáõ ùìê

    // äãôñú äâøó
   // g.printGraph();

    // äöâú ëì äîñìåìéí ìîöá èå÷ï îîöá äúçìúé îñåéí
   // g.printPathsToToken("start");  // äúçì îîöá äúçìä ùàúä øåöä
    Lexer lexer;
    //string program = lexer.readFileToString("programExample.txt");
	//string program = "print(3 + ++a*2+(a-b));";
	//string program = "print(3+5);";
    /*string program = R"(
        int x = 10;
        print(x);
        if (x > 5) {
            print("x is greater than 5");
        }
        return 0;
    )";*/
    string program_print = R"(
            print("x is greater than"+x+y*3);
    )";
	string program_if = R"(
        if ((x > 10 && y < 5) || (z == 20)) {
            print("x is greater than 10 and y is less than 5 or z is equal to 20");
         }
    elif(x<8)
        {print(t);}
    else
    {print(y);}
    
}

    )";
	string program = program_if;
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
