#pragma once
#ifndef STATE_H
#define STATE_H

#include <unordered_map>
#include <string>
#include <memory>

// מחלקת State שמייצגת מצב באוטומט
class State {
public:
    std::string name;                       // שם המצב
    bool isToken;                           // האם המצב הוא טוקן
    std::unordered_map<char, std::shared_ptr<State>> transitions; // המפות של מעבר מצבים

    State(const std::string& stateName, bool tokenFlag);
};

#endif // STATE_H
