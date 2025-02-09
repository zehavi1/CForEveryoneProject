#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <list>
enum Lexical
{
  CHAR, START,IDENTITY,ERROR1,COMMENTLINE,COMMENTLINES,ALPHA,NUMBER1,CHARACTER,NEWLINE,CHECK,MAKETOKEN,STRING
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
	 //add a char '!' to the end of the string to make sure that the last token will be added to the tokens list
     

    // shared_ptr<State> currentState = graph.getInitialState();
     string currentTokenValue="";
     bool canBeIdentifier = false;
     bool isKnownState = true;
     int lineNumber = 1;
     int countChar = 0;
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
		 if (i == input.size()-1)
			 cout << "";
         char c = input[i];
         switch (lexState)
         {
            case START:
                if (isInString == true&&c!='"')
                {
                    lexState = STRING; 
                    break;
                }
                if(c== '\''&&countChar==0)
				{
					lexState = CHAR;
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
                    lexState = NUMBER1;
                    break;
                }
                   
                if (c == '/')
                {
                    lexState=COMMENTLINE;
                    break;
                }
                    
                lexState = CHARACTER;
                break;
            case ALPHA:
            {
                if (isdigit(c))
                    lexState = IDENTITY;
                else
                if (!(isalpha(c) || c == '_'))
                    lexState = CHECK;
                else
                {
                    currentTokenValue += c;
                    i++;
                }
               break;
            }
            case IDENTITY:
            {
                if (!(isalpha(c) || c == '_' || isdigit(c)))
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
                            lexState = ERROR1;
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
            case COMMENTLINE:
            {
                lexState = ERROR1;
                break;
            }
                
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
                        lexState = ERROR1;
                        break;
                    }
                    
                }  
                i++;
                lexState = MAKETOKEN; 
                break; }
            case NUMBER1: {
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
            case CHAR:
            {
                currentTokenValue += c; i++;
                switch (countChar)
                {
                   
                case 0:
                case 1:countChar++; break;
                case 2:  pattern = TOK_CHAR; lexState = MAKETOKEN; countChar = 0; break;
                default:
                    lexState = ERROR1;
                    
                }
                break;
            }
         case ERROR1:
         default:
             list<Token> v1;
             int i = tokens.size()-1;
             while (tokens[i].typeToken != TOK_SEMICOLON)
                 v1.push_front(tokens[i--]);
             //currentTokenValue = "lexical error occured in line" + lineNumber;
             printf("lexical error occured in line %d in characters %s\n", lineNumber, currentTokenValue);
             cout << "in ";
             for (Token i :v1)
             {
                 cout << i.value<<" ";
             }
             //tokens.emplace_back(TOK_ERROR, currentTokenValue, lineNumber);
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