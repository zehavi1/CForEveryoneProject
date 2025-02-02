#include <iostream>
#include "Lexer.h"
#include "SyntacticAnalysis.h"
#include "CodeGenerator.h"

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
    string program_mini = R"(
        int x = 10;
        print(x);
        if (x > 5) {
            print("x is greater than 5");
        }
        return 0;
    )";
    string program_print = R"(
            print("x is greater than"+x+y*3+"and y="+y);
    )";
	string program_if = R"(
        if ((x > 10 && y < 5) || (z == 20)&&(x+9)<35) {
            print("x is greater than 10 and y is less than 5 or z is equal to 20");
         }
    elif(x<8)
        print(t);
    else
    {print(y);}
    
}

    )";
	string program_declaration = R"(
        int x = 10,y=40;
        double y = 3.14;
        string z = "hello";
        var b = TRUE;
        char c = 'F';
    )";
    string program_for= R"(
       for(int i=0;i<100;i++)
{
print(i);
}
    )";
    string program_while = R"(
       while(x<5){print(i);}  )";
    string program_collection = R"(
       foreach(int item in [x,y,z]){print(i);}  )";
    string program_samentic= R"(
{
        int x;
        x=7;
        if(x>5){ var y=9;
                    y+=2;
        foreach(int z in [1,2,3])
        {print(x);
        print(y);}
        }
        print(x);
        print(y);})";
	string program = program_print;
    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return 1;
    }
    program += ' ';
    auto tokens = lexer.tokenize(program);
    lexer.printTokens(tokens);
    SyntacticAnalysis parser(tokens);
    shared_ptr<ASTNode> ast = parser.parse();
    cout << "Abstract Syntax Tree:" << endl;
    ast->printASTNode();
    SemanticAnalyzer semantic;
    semantic.analyze(ast); 
    auto scopesFinal=semantic.getScopesFinal();
    CodeGenerator generator(ast,scopesFinal);
    generator.generateCode(ast);
    shared_ptr<ASTNode> astNew= generator.getNewAst();
    astNew->printASTNode();
    return 0;
}
