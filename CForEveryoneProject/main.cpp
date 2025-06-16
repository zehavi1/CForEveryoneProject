#include <iostream>
#include <string>
#include "Lexer.h"
#include "SyntacticAnalysis.h"
#include "CodeGenerator.h"
//#include "SemanticAnalyzer.h"


int main3() {
    
    Lexer lexer;
    //string program = lexer.readFileToString("programExample.txt");
	//string program = "print(3 + ++a*2+(a-b));";
	//string program = "print(3+5);";
    std::string program_mini = R"(
        int x = 10;
        print(x);
        if (x > 5) {
            print("x is greater than 5");
        }
        return 0;
////
    )";
    std::string program_print = R"(
          {
            var x=9,y=0;
            print("x is greater than "+x+y*3+" and y="+y);}
    )";
	std::string program_if = R"(
        if ((x > 10 && y < 5) || (z == 20)&&(x+9)<35) {
            print("x is greater than 10 and y is less than 5 or z is equal to 20");
         }
    elif(x<8)
        print(t);
    else
    {print(y);}
    
}

    )";
    std::string program_ifrange = R"(
       { int x=0;
        if(x<9&&5<x<10){print("x is "+x);}
}
    
}

    )";
	std::string program_declaration = R"(
    int main(int a,int b){
        int x = 10,y1=40,z1=x+y;
        double y = 3.14;
        string z = "hello";
        return 0;
        }
    )";
    std::string program_for= R"(
       for(int i=0;i<100;i++)
{
print(i);
}
    )";
    std::string program_while = R"(
       while(x<5){print(i);}  )";
    std::string program_collection = R"(
       foreach(int item in [x,y,z]){print(i);}  )";
    std::string program_samentic= R"(
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
    std::string program_array = R"(
void main(){
        int [] arr=new int[3];
        char* arr2 = malloc(4);
        double arr3[4]={1,2,3};
        string s="abcd";
}
         )";
    std::string program_full = R"(
void main(){
        int [] arr=new int[3];
        char* arr2 = malloc(4);
        int n=3;
foreach(int i in arr,n){
        print(i);
}
        double arr3[4]={1,2,3};
        string s="abcd";
        int x=8;
        if(x>5){
            print("x is greater than 5");
}
        bool flag=true;
        var x=8,y=9;
        if(6<x<10)
            print("x is"+x+"and y is"+y+"and x+y equals"+x+y);
        elif(y<4||flag)
            print("y<4"+"flag is:"+flag);
        else
            print("oooops...");
}
         )";
    std::string program = program_full;
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
    CodeGenerator generator(ast, tokens);
    generator.CodeGenerator_main();
    //generator.generateCode(ast);
    shared_ptr<ASTNode> &astNew= generator.getNewAst();
    astNew->printASTNode();
    cout <<"base program:"<<endl << program << endl;
    cout<<"profram in c:"<<endl << astNew->printOriginalCode(0);
    return 0;
}
