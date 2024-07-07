#pragma once

#include "raylib.h"
#include <string>
#include <vector>

float step(float val, float target, float step);
int istep(int val, int target, int step);

int round_to(int num, int multiple);

std::string int_to_str(int val, int len);

inline void trace(const char* msg)
{
    TraceLog(TraceLogLevel::LOG_INFO, msg);
}

bool are_keys_down(std::vector<int> keys);
bool are_keys_pressed(std::vector<int> keys);
