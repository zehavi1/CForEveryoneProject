#include "graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>

// קריאת הנתונים מהקובץ CSV
void Graph::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string state, symbol, nextState, isToken;
        std::getline(ss, state, ',');
        std::getline(ss, symbol, ',');
        std::getline(ss, nextState, ',');
        std::getline(ss, isToken, ',');  // טור חדש לציון אם המצב הוא טוקן

        // יצירת המצב אם לא קיים
        if (states.find(state) == states.end()) {
            states[state] = std::make_shared<State>(state, isToken == "y");
        }
        if (states.find(nextState) == states.end()) {
            states[nextState] = std::make_shared<State>(nextState, isToken == "y");
        }

        // הוספת הקשת לגרף
        states[state]->transitions[symbol[0]] = states[nextState];
    }
    setInitialState();
}

// הדפסת כל המצבים עם מעבריהם
void Graph::printGraph() const {
    for (const auto& pair : states) {
        const auto& state = pair.second;
        std::cout << "State: " << state->name << (state->isToken ? " (Token)" : "") << "\n";
        for (const auto& transition : state->transitions) {
            std::cout << "  Symbol: " << transition.first
                << " -> Next State: " << transition.second->name << "\n";
        }
    }
}

// הצגת המסלולים למצב טוקן מסוים
void Graph::printPathsToToken(const std::string& startState) {
    std::unordered_map<std::string, bool> visited;
    std::queue<std::pair<std::shared_ptr<State>, std::vector<std::string>>> q;
    std::shared_ptr<State> start = states[startState];
    q.push(std::make_pair(start, std::vector<std::string>{start->name}));

    while (!q.empty()) {
        std::pair<std::shared_ptr<State>, std::vector<std::string>> front = q.front();
        q.pop();

        std::shared_ptr<State> currentState = front.first;  // המצב הנוכחי
        std::vector<std::string> path = front.second;  // מסלול

        if (currentState->isToken) {
            std::cout << "Path to Token (" << currentState->name << "): ";
            for (const auto& state : path) {
                std::cout << state << " -> ";
            }
            std::cout << "\n";
        }

        for (const auto& transition : currentState->transitions) {
            if (visited.find(transition.second->name) == visited.end()) {
                visited[transition.second->name] = true;
                std::vector<std::string> newPath = path;
                newPath.push_back(transition.second->name);
                q.push(std::make_pair(transition.second, newPath));
            }
        }
    }

}
shared_ptr<State> Graph::getState(const string& stateName) {
    if (states.count(stateName)) {
        return states[stateName];
    }
    return nullptr; // אם המצב לא קיים, מחזירים nullptr
}
void Graph::setInitialState() {
    string stateName = "start";
    auto state = getState(stateName);
    if (state != nullptr) {
        initialState = state;
    }
    else {
        cerr << "Error: State " << stateName << " does not exist!" << endl;
    }
}


