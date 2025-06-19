#pragma once
#include <iostream>  
#include <map>  
#include <vector>  
#include "enums.h"
#include <regex>
#include "Token.h"
#include <list>
using namespace std;
class LexicalAnalyzer
{
private:
    enum LexicalCategory {
        ALPHA_CAT, DIGIT_CAT, DOT_CAT, UNDERSCORE_CAT, SPACE_CAT, DOUBLE_QUOTE_CAT, SINGLE_QUOTE_CAT, NEWLINE_CAT, OTHER_CAT
    };

    enum LexicalState {
        ERROR1, START, CHAR, IDENTITY, ALPHA, NUMBER1, NUMBERDOUBLE, STRING
    };
    vector<Token> tokens;
    bool isInString = false;
	string input;
    const vector<string> multiCharTokens = { "==", "!=", ">=", "<=", "&&", "||","++","--" }; // טוקנים מרובי-תווים
    const vector<char> singleCharTokens = { ';', ',', '{', '}', '(', ')', '+', '-', '*', '/', '=', '<', '>','"','[',']','%' }; // טוקנים חד-תווים
    const std::map<std::string, Pattern> mapSaved = mapAlphaTokens;
    const std::map<std::string, Pattern> mapSavedNonAlphaTokens = mapNonAlphaTokens;
    std::map<LexicalState, std::map<LexicalCategory, void(LexicalAnalyzer::*)()>> transitionMatrix;

    std::map<char, LexicalCategory> charToLexical;

    bool canBeIdentifier = false;
    bool isKnownState = true;
    string currentTokenValue = "";
    int lineNumber = 1;
    int countChar = 0;
    size_t i = 0;
    char c = input[i];
    LexicalState lexState = START;
    Pattern pattern = TOK_ERROR;
public:
	LexicalAnalyzer(const string& input) : input(input) {
		initializeCharToLexicalMap();
		initializeTransitionMatrix();
		main_LexicalAnalyzer();
	}
    void main_LexicalAnalyzer()
    {
            c = input[i];
            LexicalCategory category = charToLexical[c];
            //if (transitionMatrix[lexState].find(category) != transitionMatrix[lexState].end()) {
                (this->*transitionMatrix[lexState][category])();
           // }
        
    }
    void printTokens(const vector<Token>& tokens) const {
        for (const auto& token : tokens) {
            cout << "Type: " << tokenNames[token.typeToken] << ", Value: " << token.value << ", line:" << token.lineNumber << endl;
        }
    }
	vector<Token> getTokens() const{
		return tokens;
	}
	

	
    void next()
    {
		int len = input.length();
        c = input[i];
        LexicalCategory category = charToLexical[c];
        i < len ? (this->*transitionMatrix[lexState][category])() : endLex(); 
    }
    void endLex()
    {
		printf("Lexical analysis has done without errors\n");
    }
	void initializeCharToLexicalMap() {
		for (char c = 'a'; c <= 'z'; ++c) {
			charToLexical[c] = ALPHA_CAT;
		}

		for (char c = '0'; c <= '9'; ++c) {
			charToLexical[c] = DIGIT_CAT;
		}

		charToLexical['_'] = UNDERSCORE_CAT;
		charToLexical[' '] = SPACE_CAT;
		charToLexical['"'] = DOUBLE_QUOTE_CAT;
		charToLexical['\''] = SINGLE_QUOTE_CAT;
		charToLexical['\n'] = NEWLINE_CAT;
        charToLexical['.'] = DOT_CAT;

		for (int c = 0; c < 256; ++c) {
			if (charToLexical.find(c) == charToLexical.end()) {
				charToLexical[c] = OTHER_CAT;
			}
		}
	}
	

	/*LexicalState getNextState(LexicalState currentState, char inputChar) {
		LexicalCategory category = charToLexical[inputChar];
		return transitionMatrix[currentState][category];
	}*/
    void handle_error_cat() {
        lexState = ERROR1;
        list<Token> v1;
        int i = tokens.size() - 1;
        while (tokens[i].typeToken != TOK_SEMICOLON)
            v1.push_front(tokens[i--]);
        //currentTokenValue = "lexical error occured in line" + lineNumber;
        printf("lexical error occured in line %d in characters %s\n", lineNumber, currentTokenValue);
        cout << "in ";
        for (Token i : v1)
        {
            cout << i.value << " ";
        }
        //tokens.emplace_back(TOK_ERROR, currentTokenValue, lineNumber);
        throw tokens;
    }
    void makeToken()
    {
        tokens.emplace_back(pattern, currentTokenValue, lineNumber);
        pattern = TOK_ERROR;
        currentTokenValue = "";
    }
    // פונקציות לכל שילוב של מצב וקטגוריה
    void handle_start_alpha_cat() {
        lexState = ALPHA;
        next();
    }

    void handle_start_digit_cat() {
        lexState = NUMBER1;
        next();
    }

    void handle_start_underscore_cat() {
        lexState = ALPHA;
        next();
    }

    void handle_start_space_cat() {
        i++;
        next();
    }

    void handle_start_double_quote_cat() {
		string s2 = { c };
		pattern = TOK_STRING_LITERAL;
		currentTokenValue = s2;
        makeToken();
		lexState = STRING;
		i++;
        next();
    }

    void handle_start_single_quote_cat() {
        lexState = CHAR;
        next();
    }

    void handle_start_newline_cat() {
        i++;
        lineNumber++;
        next();
    }

    void handle_start_other_cat() {
        lexState = CHAR;
        next();
    }

    // פונקציות עבור מצב CHAR
    void handle_char_alpha_cat() {
        currentTokenValue += c;
        i++;
        next();
    }

    void handle_char_digit_cat() {
        lexState = IDENTITY;
        next();
    }

    void handle_char_underscore_cat() {
        currentTokenValue += c;
        i++;
        next();
    }

    //void handle_char_space_cat() {
    //   
    //}

    //void handle_char_double_quote_cat() {
    //    // מימוש ריק
    //}

    /*void handle_char_single_quote_cat() {
       
    }*/

    /*void handle_char_newline_cat() {
        lineNumber++;
        next();
    }*/

    void handle_char_other_cat() {
       
			string s2 = { c,input[i + 1] };
			string s3 = { c };
			if (mapSavedNonAlphaTokens.find(s2) != mapSavedNonAlphaTokens.end()) {
				pattern = mapSavedNonAlphaTokens.at(s2);
				currentTokenValue = s2;
				i+=2;
                makeToken();
                lexState = START;
			}
			else
			{
				if (mapSavedNonAlphaTokens.find(s3) != mapSavedNonAlphaTokens.end()) {
					pattern = mapSavedNonAlphaTokens.at(s3);
					currentTokenValue = s3;
                    i++;
                    makeToken();
                    lexState = START;
				}
				else
				{
					lexState = ERROR1;
				}
			}
        next();
    }

    // פונקציות עבור מצב IDENTITY
    void handle_identity_alpha_digit_underscore_cat() {
        currentTokenValue += c;
        i++; next();
    }

    //void handle_identity_digit_cat() {
    //    currentTokenValue += c;
    //    i++;
    //}

    //void handle_identity_underscore_cat() {
    //    // מימוש ריק
    //}

    //void handle_identity_slash_cat() {
    //    // מימוש ריק
    //}

    //void handle_identity_double_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_identity_single_quote_cat() {
    //    // מימוש ריק
    //}

    /*void handle_identity_newline_cat() {
        lineNumber++;
    }*/

    void handle_identity_other_cat() {
        pattern = TOK_ID;
        makeToken();
        lexState = START;
        next();
    }

    // פונקציות עבור מצב ALPHA
    void handle_alpha_alpha_underscore_cat() {
        currentTokenValue += c;
        i++;
        next();
    }

    void handle_alpha_digit_cat() {
        lexState = IDENTITY;
        next();
    }

    //void handle_alpha_underscore_cat() {
    //    // מימוש ריק
    //}

    //void handle_alpha_slash_cat() {
    //    // מימוש ריק
    //}

    //void handle_alpha_double_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_alpha_single_quote_cat() {
    //    // מימוש ריק
    //}

    /*void handle_alpha_newline_cat() {
        lineNumber++;
    }*/

    void handle_alpha_other_cat() {
        if (mapSaved.find(currentTokenValue) != mapSaved.end()) {
            pattern = mapSaved.at(currentTokenValue);
        }
        if (pattern == TOK_ERROR)
        {
            pattern = TOK_ID;

        }
        makeToken();
        lexState = START;
        next();
    }

    // פונקציות עבור מצב NUMBER1
    void handle_number1_alpha_underscore_cat() {
        lexState = ERROR1;
        next();
    }

    void handle_number1_digit_cat() {
        currentTokenValue += c;
        i++;
        next();
        
    }

    void handle_number1_dot_cat() {
        currentTokenValue += c;
        i++;
        lexState = NUMBERDOUBLE;
        next();
    }
    //void handle_number1_underscore_cat() {
    //    // מימוש ריק
    //}

    //void handle_number1_slash_cat() {
    //    // מימוש ריק
    //}

    //void handle_number1_double_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_number1_single_quote_cat() {
    //    // מימוש ריק
    //}

    /*void handle_number1_newline_cat() {
        lineNumber++;
    }*/

    void handle_number1_other_cat() {
        pattern = TOK_INT;
        makeToken();
        lexState = START;
        next();
    }
    void handle_numberdouble_digit_cat() {
        currentTokenValue += c;
        i++;
        next();

    }
    void handle_numberdouble_other_cat() {
        pattern = TOK_DOUBLE;
        makeToken();
        lexState = START;
        next();
    }
    // פונקציות עבור מצב CHARACTER
    void handle_character_alpha_cat() {
        lexState = ERROR1;
        next();
    }

    void handle_character_digit_cat() {
        lexState = ERROR1;
        next();
    }

    void handle_character_underscore_cat() {
        lexState = ERROR1;
        next();
    }

    void handle_character_slash_cat() {
        lexState = ERROR1;
        next();
    }

    //void handle_character_double_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_character_single_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_character_newline_cat() {
    //    // מימוש ריק
    //}

    void handle_character_other_cat() {
        string s2 = { c,input[i + 1] };
        string s3 = { c };
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
            }

        }
        i++;
        makeToken();
        lexState = START;
    }

    
    // פונקציות עבור מצב STRING
    //void handle_string_alpha_cat() {
    //    // מימוש ריק
    //}

    //void handle_string_digit_cat() {
    //    // מימוש ריק
    //}

    //void handle_string_underscore_cat() {
    //    // מימוש ריק
    //}

    //void handle_string_slash_cat() {
    //    // מימוש ריק
    //}

    void handle_string_double_quote_cat() {
        pattern = TOK_STRING;
        makeToken();
        pattern = TOK_STRING_LITERAL;
		currentTokenValue = input[i];
        makeToken();
        i++;
		lexState = START;
		next();
    }

    //void handle_string_single_quote_cat() {
    //    // מימוש ריק
    //}

    //void handle_string_newline_cat() {
    //    // מימוש ריק
    //}

    void handle_string_other_cat() {
        currentTokenValue += c;
        i++;
        next();
    }
    void initializeTransitionMatrix() {
        transitionMatrix[START][ALPHA_CAT] = &LexicalAnalyzer::handle_start_alpha_cat;
        transitionMatrix[START][DIGIT_CAT] = &LexicalAnalyzer::handle_start_digit_cat;
        transitionMatrix[START][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_start_underscore_cat;
        transitionMatrix[START][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_start_double_quote_cat;
        transitionMatrix[START][SPACE_CAT] = &LexicalAnalyzer::handle_start_space_cat;
        transitionMatrix[START][DOT_CAT] = &LexicalAnalyzer::handle_start_other_cat;
        transitionMatrix[START][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_start_other_cat;   
        transitionMatrix[START][NEWLINE_CAT] = &LexicalAnalyzer::handle_start_newline_cat;
        transitionMatrix[START][OTHER_CAT] = &LexicalAnalyzer::handle_start_other_cat;
		transitionMatrix[ALPHA][ALPHA_CAT] = &LexicalAnalyzer::handle_alpha_alpha_underscore_cat;
		transitionMatrix[ALPHA][DIGIT_CAT] = &LexicalAnalyzer::handle_alpha_digit_cat;
		transitionMatrix[ALPHA][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_alpha_alpha_underscore_cat;
        transitionMatrix[ALPHA][SPACE_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[ALPHA][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[ALPHA][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[ALPHA][NEWLINE_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[ALPHA][OTHER_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[ALPHA][DOT_CAT] = &LexicalAnalyzer::handle_alpha_other_cat;
		transitionMatrix[NUMBER1][ALPHA_CAT] = &LexicalAnalyzer::handle_number1_alpha_underscore_cat;
		transitionMatrix[NUMBER1][DIGIT_CAT] = &LexicalAnalyzer::handle_number1_digit_cat;
		transitionMatrix[NUMBER1][DOT_CAT] = &LexicalAnalyzer::handle_number1_dot_cat;
		transitionMatrix[NUMBER1][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_number1_alpha_underscore_cat;
        transitionMatrix[NUMBER1][SPACE_CAT] = &LexicalAnalyzer::handle_number1_other_cat;
		transitionMatrix[NUMBER1][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_number1_other_cat;
		transitionMatrix[NUMBER1][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_number1_other_cat;
		transitionMatrix[NUMBER1][NEWLINE_CAT] = &LexicalAnalyzer::handle_number1_other_cat;
		transitionMatrix[NUMBER1][OTHER_CAT] = &LexicalAnalyzer::handle_number1_other_cat;
		transitionMatrix[NUMBERDOUBLE][DIGIT_CAT] = &LexicalAnalyzer::handle_numberdouble_digit_cat;
		transitionMatrix[NUMBERDOUBLE][ALPHA_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][DOT_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][SPACE_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[NUMBERDOUBLE][NEWLINE_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
        transitionMatrix[NUMBERDOUBLE][OTHER_CAT] = &LexicalAnalyzer::handle_numberdouble_other_cat;
		transitionMatrix[CHAR][ALPHA_CAT] = &LexicalAnalyzer::handle_char_alpha_cat;
		transitionMatrix[CHAR][DIGIT_CAT] = &LexicalAnalyzer::handle_char_digit_cat;
		transitionMatrix[CHAR][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_char_underscore_cat;
		transitionMatrix[CHAR][SPACE_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[CHAR][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[CHAR][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[CHAR][NEWLINE_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[CHAR][DOT_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[CHAR][OTHER_CAT] = &LexicalAnalyzer::handle_char_other_cat;
		transitionMatrix[IDENTITY][ALPHA_CAT] = &LexicalAnalyzer::handle_identity_alpha_digit_underscore_cat;
		transitionMatrix[IDENTITY][DIGIT_CAT] = &LexicalAnalyzer::handle_identity_alpha_digit_underscore_cat;
		transitionMatrix[IDENTITY][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_identity_alpha_digit_underscore_cat;
		transitionMatrix[IDENTITY][SPACE_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[IDENTITY][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[IDENTITY][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[IDENTITY][NEWLINE_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[IDENTITY][DOT_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[IDENTITY][OTHER_CAT] = &LexicalAnalyzer::handle_identity_other_cat;
		transitionMatrix[STRING][ALPHA_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][DIGIT_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][SPACE_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][DOT_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_string_double_quote_cat;
		transitionMatrix[STRING][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][NEWLINE_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[STRING][OTHER_CAT] = &LexicalAnalyzer::handle_string_other_cat;
		transitionMatrix[ERROR1][ALPHA_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][DIGIT_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][DOT_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][UNDERSCORE_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][SPACE_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][DOUBLE_QUOTE_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][SINGLE_QUOTE_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][NEWLINE_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][OTHER_CAT] = &LexicalAnalyzer::handle_error_cat;
		transitionMatrix[ERROR1][DOT_CAT] = &LexicalAnalyzer::handle_error_cat;
    }
};




//int main() {
//    initializeCharToLexicalMap();

//    // דוגמא לשימוש במפה
//    char testChar = 'a';
//    if (charToLexical.find(testChar) != charToLexical.end()) {
//        std::cout << "The Lexical category of '" << testChar << "' is " << charToLexical[testChar] << std::endl;
//    }
//    else {
//        std::cout << "Character not found in map." << std::endl;
//    }

//    return 0;
//}

