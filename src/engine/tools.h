#pragma once

#include "raylib.h"
#include <string>
#include <vector>

float step(float val, float target, float step);

std::string int_to_str(int val, int len);

inline void trace(const char* msg)
{
    TraceLog(TraceLogLevel::LOG_INFO, msg);
}

class Logger {

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
