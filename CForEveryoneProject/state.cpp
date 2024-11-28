#include "state.h"

// בונה מצב חדש
State::State(const std::string& stateName, bool tokenFlag)
    : name(stateName), isToken(tokenFlag) {}
