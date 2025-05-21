#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include "Lexer.h"
#include "SyntacticAnalysis.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"

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

int main(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <path_to_input_file>" << endl;
        return 1;
    }
    string inputFilePath = argv[1];
    string outputFilePath = inputFilePath.substr(0, inputFilePath.find_last_of('.')) + ".c";

    string program = readProgramFromFile(inputFilePath);
    if (program.empty()) return 1;

    Lexer lexer;
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
    shared_ptr<ASTNode>& astNew = generator.getNewAst();
    astNew->printASTNode();

    cout << "base program:" << endl << program << endl;
    string finalCode = astNew->printOriginalCode(0);
    cout << "program in C:" << endl << finalCode;

    writeOutputToFile(outputFilePath, finalCode);

    return 0;
}
