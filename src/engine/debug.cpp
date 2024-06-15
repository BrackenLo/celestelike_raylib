#include "debug.h"

#ifdef DEBUG

#include "raylib.h"
// #include <raygui>
#include "raygui.h"

void Debugger::render()
{
    for (int x = 0; x < messages.size(); x++) {
        DrawText(messages[x].c_str(), 20, 20 + 20 * x, 20, BLACK);
    }

    messages.clear();
}

#endif
