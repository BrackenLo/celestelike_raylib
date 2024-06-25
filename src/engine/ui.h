#pragma once

#include "debug.h"
#include "raygui.h"
#include <vector>

int GuiPropertyListView(Rectangle bounds, std::vector<DebugProperty> properties, int* scroll_index);
