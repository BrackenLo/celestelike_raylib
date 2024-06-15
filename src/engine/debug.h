#pragma once

#include <string>
#include <vector>

#define DEBUG

#ifdef DEBUG

class Debugger {
public:
    inline void add_message(const char* text)
    {
        messages.push_back(std::string(text));
    }

private:
    void render();
    friend class World;

private:
    std::vector<std::string> messages;
};

#endif
