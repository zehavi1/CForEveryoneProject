#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#define asString(a) #a
enum Lexical
{
   START,IDENTITY,ERROR,COMMENTLINE,COMMENTLINES,ALPHA,NUMBER,CHARACTER,NEWLINE,CHECK,MAKETOKEN,STRING
};
string Lexer::readFileToString(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

vector<Token> Lexer::tokenize(const string& input) {
     vector<Token> tokens;
     bool isInString = false;
     Pattern pattern = TOK_ERROR;
    // shared_ptr<State> currentState = graph.getInitialState();
     string currentTokenValue="";
     bool canBeIdentifier = false;
     bool isKnownState = true;
     int lineNumber = 1;
     size_t i = 0;
     Lexical lexState = START;
     regex number_regex(R"(\d+)");
     regex letter_regex(R"([a-zA-Z]+)");
     regex valid_var_regex(R"([a-zA-Z_][a-zA-Z0-9_]*$)");
     regex double_regex(R"(^-?\d+(\.\d+)?$)"); // מאפשר גם שלמים וגם עשרוניים
     regex int_regex(R"(^-?\d+$)"); // מספרים שלמים
     regex stringRegex("^\".*\"$");
     for (int i=0;i<input.size();)
     {
         char c = input[i];
         switch (lexState)
         {
            case START:
                if (isInString == true&&c!='"')
                {
                    lexState = STRING; 
                    break;
                }
                   
                   
                if (isWhitespace(c)) {
                    i++;
                    break;
                }
                if (c == '\n') {
                    i++;
                    lineNumber++; 
                    
                    break;
                }
                if (isalpha(c) || c == '_')
                {
                    lexState = ALPHA;
                    break;
                }
                    
                if (isdigit(c))
                {
                    lexState = NUMBER;
                    break;
                }
                   
                if (c == '/')
                {
                lexState:COMMENTLINE;
                    break;
                }
                    
                lexState = CHARACTER;
                break;
            case ALPHA:
            {
                if (!(isalpha(c) || c == '_'))
                    lexState = CHECK;
                else
                {
                    currentTokenValue += c;
                    i++;
                }
               break;
            }
            case CHECK:
            {
                if (!(isalpha(c) || c == '_' || isdigit(c)))
                {
                    //pattern = TOK_ERROR;
                    if (mapSaved.find(currentTokenValue) != mapSaved.end()) {
                        pattern = mapSaved.at(currentTokenValue);
                    }
                    if (pattern == TOK_ERROR)
                    {
                        if (regex_match(currentTokenValue, valid_var_regex))
                            pattern = TOK_ID;
                        else
                        {
                            lexState = ERROR;
                            break;
                        }
                           
                    }
                    lexState = MAKETOKEN;
                }
                break;
            }
            case MAKETOKEN:
            {
                
                tokens.emplace_back(pattern, currentTokenValue, lineNumber);
                pattern = TOK_ERROR;
                currentTokenValue = "";
                lexState = START;
                break;
            }
            case COMMENTLINE:break;
            case STRING:
            {
                if (c == '"')
                {
                    
                    pattern = TOK_STRING;
                    lexState = MAKETOKEN;
                }
                else
                {
                    currentTokenValue += c;
                    i++;
                }
                break;

            }
            case CHARACTER: {
                if (c == '"')
                    isInString = !isInString;
                string s2 = { c,input[i + 1] };
                string s3 = {c};
                if (mapNonAlphaTokens.find(s2) != mapNonAlphaTokens.end()) {
                    pattern = mapNonAlphaTokens.at(s2);
                    currentTokenValue = s2;
                    i++;
                }
                else
                {
                    if (mapNonAlphaTokens.find(s3) != mapNonAlphaTokens.end()) {
                        pattern = mapNonAlphaTokens.at(s3);
                        currentTokenValue = s3;
                    }
                    else
                    {
                        lexState = ERROR;
                        break;
                    }
                    
                }  
                i++;
                lexState = MAKETOKEN; 
                break; }
            case NUMBER: {
                if (regex_match(currentTokenValue + c,double_regex)||(regex_match(currentTokenValue, int_regex)&&c=='.'))
                {
                    currentTokenValue += c;
                    i++;
                }
                else
                {
                    if (regex_match(currentTokenValue, int_regex))
                        pattern = TOK_INT;
                    else
                        pattern = TOK_DOUBLE;
                    lexState = MAKETOKEN;
                }
                break;
            }
         case ERROR:
         default:
             printf("lexical error occured in line %d in characters %s\n", lineNumber, currentTokenValue);
             return tokens;
             break;
         }
     }
     return tokens;
 }

void Lexer::printTokens(const vector<Token>& tokens) const {
    for (const auto& token : tokens) {
        cout << "Type: " <<tokenNames[ token.typeToken] << ", Value: " << token.value << ", line:" << token.lineNumber << endl;
    }
}