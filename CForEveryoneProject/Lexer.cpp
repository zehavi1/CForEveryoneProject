#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>

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

vector<Token> Lexer::tokenize(const string& input, const Graph& graph) {
    vector<Token> tokens;
    shared_ptr<State> currentState = graph.getInitialState();
    string currentTokenValue;
    bool canBeIdentifier = false;
    bool isKnownState = true;
    int lineNumber = 1;
    size_t i = 0;
    while (i < input.size()) {
        char c = input[i];
        // דילוג על רווחים ולבנים
        if (isWhitespace(c)) {
            i++;
            continue;
        }
        
        // עדכון מספר שורה
        if (c == '\n') {
            lineNumber++;
            i++;
            continue;
        }
        
        // זיהוי מספרים
        if (isDigit(c)) {
            
            string number;
            bool hasDot = false;
            while (i < input.size() && (isDigit(input[i]) || input[i] == '.')) {
                if (input[i] == '.') {
                    if (hasDot) break; // יותר מנקודה אחת לא תקין במספר
                    hasDot = true;
                }
                number += input[i++];
            }
            tokens.emplace_back("Number", number,lineNumber);
            continue;
        }
        
        // זיהוי מזהים או טוקנים מבוססי מצבים
        if (isIdentifierStart(c)) {
            isKnownState = true;
            currentTokenValue.clear();
            currentState = graph.getInitialState();
            canBeIdentifier = true;

            while (i < input.size() && isIdentifierPart(input[i])&&isKnownState) {
                char ch = input[i++];
                currentTokenValue += ch;
                if (currentState->transitions.count(ch)) {
                    currentState = currentState->transitions[ch];
                }
                else {
                    isKnownState = false;
                }
            }

            if (currentState->isToken) {
                tokens.emplace_back(currentState->name, currentTokenValue,lineNumber);
            }
            else if (canBeIdentifier) {
                while (canBeIdentifier)
                {
                    char ch = input[i++];
                    if (isalnum(ch))
                        currentTokenValue += ch;
                    else
                        canBeIdentifier = false;

                }
                tokens.emplace_back("Identifier", currentTokenValue,lineNumber);
            }
            else {
                tokens.emplace_back("Error", currentTokenValue,lineNumber);
            }
            continue;
        }

        // זיהוי טוקנים של סימני פיסוק
        if (find(singleCharTokens.begin(), singleCharTokens.end(), c) != singleCharTokens.end()) {
            string singleToken(1, c);

            // בדיקה לטוקנים מרובי-תווים
            if (i + 1 < input.size()) {
                string potentialMultiToken = singleToken + input[i + 1];
                if (isMultiCharToken(potentialMultiToken)) {
                    tokens.emplace_back("Operator", potentialMultiToken,lineNumber);
                    i += 2;
                    continue;
                }
            }

            // טוקן חד-תווי
            tokens.emplace_back("Punctuation", singleToken,lineNumber);
            i++;
            continue;
        }

        // אם התו לא מזוהה
        tokens.emplace_back("Error", string(1, c),lineNumber);
        i++;
    }

    return tokens;
}

void Lexer::printTokens(const vector<Token>& tokens) const {
    for (const auto& token : tokens) {
        cout << "Type: " << token.type << ", Value: " << token.value<<"line:"<<token.lineNumber << endl;
    }
}
