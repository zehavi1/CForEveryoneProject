#pragma once
#include <iostream>
#include <fstream>
#include <memory>
//#include "Lexer.h"
#include "SyntacticAnalysis.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"
#include "LexicalAnalyzer.h"

using namespace std;

string readProgramFromFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open program file!" << endl;
        return "";
    }

    string program((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return program;
}

void writeOutputToFile(const string& outputPath, const string& output) {
    ofstream file(outputPath);
    if (!file.is_open()) {
        cerr << "Failed to open output file!" << endl;
        return;
    }
    file << output;
    file.close();
}
string mainCompilerForDev(std::string program)
{
    //Lexer lexer;
    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return "";
    }
    program += ' ';
    LexicalAnalyzer lexer(program);
    vector<Token> tokens = lexer.getTokens();
    cout << "Tokens:" << endl;

    // auto tokens = lexer.tokenize(program);
    lexer.printTokens(tokens);
    SyntacticAnalysis parser(tokens);
    shared_ptr<ASTNode> ast = parser.parse();
    cout << "Abstract Syntax Tree:" << endl;
    ast->printASTNode();
    SemanticAnalyzer semantic;
    semantic.analyze(ast);
    CodeGenerator generator(ast, tokens);
    generator.CodeGenerator_main();
    shared_ptr<ASTNode>& astNew = generator.getNewAst();
    astNew->printASTNode();
    cout << "base program:" << endl << program << endl;
    string code = astNew->printOriginalCode(0);
    cout << "profram in c:" << endl << code;
    return code;
}
std::string mainCompiler(std::string program) {
    if (program.empty()) {
        cerr << "Failed to read program file!" << endl;
        return "";
    }
    program += ' ';
	LexicalAnalyzer lexer(program);
	vector<Token> tokens = lexer.getTokens();
    SyntacticAnalysis parser(tokens);
    shared_ptr<ASTNode> ast = parser.parse();
    SemanticAnalyzer semantic;
    semantic.analyze(ast);
    CodeGenerator generator(ast, tokens);
    generator.CodeGenerator_main();
    shared_ptr<ASTNode>& astNew = generator.getNewAst();
    //astNew->printASTNode();
    string code = astNew->printOriginalCode(0);
    return code;
}
int main(int argc, char* argv[])
{
    if (argc != 3&&argc!=2) {
        cerr << "Usage: " << argv[0] << " <path_to_input_file>" << endl;
        return 1;
    }
    
    string inputFilePath = argv[1];
    string outputFilePath = inputFilePath.substr(0, inputFilePath.find_last_of('.')) + ".c";
    string status = argc == 3 ? argv[2] : "1";
    int statusNum = std::atoi(status.c_str());
    string(*compilerArr[])(string) = { mainCompilerForDev, mainCompiler };
    try {
        string program = readProgramFromFile(inputFilePath);
        string finalCode =compilerArr[statusNum](program);
        writeOutputToFile(outputFilePath, finalCode);
		cout << "Compilation successful! Output written to: " << outputFilePath << endl;
    }
	catch (const ErrorHandler& e) {
		cerr << "Error: " << e.what() << endl;
		return 1;
	}
	catch (const runtime_error& e) {
		cerr << "Runtime error: " << e.what() << endl;
		return 1;
	}
	catch (const exception& e) {
		cerr << "An error occurred: " << e.what() << endl;
		return 1;
	}
    

    return 0;
}
