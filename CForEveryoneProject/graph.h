#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include "State.h"
#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include <memory>

class Graph {
public:
    std::unordered_map<std::string, std::shared_ptr<State>> states;  // כל המצבים בגרף

    // קריאת הנתונים מהקובץ
    void loadFromCSV(const std::string& filename);

    // הדפסת הגרף
    void printGraph() const;

    // הצגת כל המסלולים למצב טוקן מסוים
    void printPathsToToken(const std::string& startState);
};

#endif // GRAPH_H

