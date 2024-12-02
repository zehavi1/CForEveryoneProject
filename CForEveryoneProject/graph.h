#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "State.h"

using namespace std;

class Graph {
private:
    unordered_map<string, shared_ptr<State>> states; // כל המצבים באוטומט
    shared_ptr<State> initialState;                 // המצב ההתחלתי

public:
    // פונקציות קיימות
    void loadFromCSV(const string& filename);
    void printGraph() const;
    void printPathsToToken(const std::string& startState);
    shared_ptr<State> getState(const string& stateName);

    // פונקציה להחזיר מצב התחלתי
    shared_ptr<State> getInitialState() const { return initialState; }

    // פונקציה לקבוע את המצב ההתחלתי
    void setInitialState();
};
