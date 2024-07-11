#pragma once

#include "debug_x.hpp"
#include "raygui.h"
#include <vector>

int GuiPropertyListView(Rectangle bounds, std::vector<DebugProperty> properties, int* scroll_index);

void DrawIntSpinner(Rectangle bounds, int* pointer, DebugProperty* properties);
void DrawFloatSpinner(Rectangle bounds, float* pointer, DebugProperty* properties);
