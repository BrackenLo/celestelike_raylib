#include "ui.hpp"

#include "debug.hpp"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include <variant>

int GuiPropertyListView(Rectangle bounds, std::vector<DebugProperty> properties, int* scroll_index)
{
    int total_lines = 0;

    struct VisitPropertyHeight {
        int operator()(int*) { return 1; }
        int operator()(bool*) { return 1; }
        int operator()(float*) { return 1; }
        int operator()(Vector2*) { return 2; }
    };

    for (DebugProperty property : properties) {
        int height = std::visit(VisitPropertyHeight(), property.property_type);
        total_lines += height;
    }

    const int list_items_spacing = GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
    const int list_items_height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    const int default_border_width = GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int list_scrollbar_width = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    const int default_text_padding = GuiGetStyle(DEFAULT, TEXT_PADDING);

    // bool use_scroll_bar = (list_items_height + list_items_spacing * total_lines) > bounds.height;

    // Calculate size of property box
    Rectangle item_bounds = { 0 };
    item_bounds.x = bounds.x + list_items_spacing;
    item_bounds.y = bounds.y + list_items_spacing + default_border_width;
    item_bounds.width = bounds.width - 2 * list_items_spacing - default_border_width;
    item_bounds.height = list_items_height;

    // TODO - Property scrolling
    // if (use_scroll_bar)
    //     item_bounds.width -= list_scrollbar_width;

    // Get items on list
    // int visible_items = (int)bounds.height / (list_items_height + list_items_spacing);
    // if (visible_items > total_lines)
    //     visible_items = total_lines;

    // int start_index = (scroll_index == nullptr) ? 0 : *scroll_index;
    // if (start_index < 0 || start_index > (total_lines - visible_items))
    //     start_index = 0;
    // int end_index = start_index + visible_items;

    GuiGroupBox(bounds, NULL);

    Rectangle item_inner_bounds = { 0 };
    item_inner_bounds.x = item_bounds.x + default_border_width;
    item_inner_bounds.width = item_bounds.width - 2 * default_border_width - 2 * default_text_padding;
    item_inner_bounds.height = item_bounds.height - 2 * default_border_width - 2 * default_text_padding;

    const float default_label_width = item_inner_bounds.width / 2;
    const float default_value_width = item_inner_bounds.width - default_label_width;
    const float default_value_x = item_inner_bounds.x + default_value_width;

    for (int i = 0; i < properties.size(); i++) {
        item_inner_bounds.y = item_bounds.y + default_border_width + default_text_padding;
        DebugProperty* property = &properties[i];

        Rectangle default_value_bounds = {
            default_value_x,
            item_inner_bounds.y,
            default_value_width,
            item_inner_bounds.height
        };

        bool default_box = true;
        bool default_label = true;

        // int
        if (std::holds_alternative<int*>(property->property_type)) {
            int* value = get<int*>(property->property_type);

            if (property->can_edit)
                DrawIntSpinner(
                    default_value_bounds,
                    value,
                    property);

            else
                GuiStatusBar(default_value_bounds, std::to_string(*value).c_str());
        }

        // bool - TODO
        else if (std::holds_alternative<bool*>(property->property_type)) {
            bool* value = get<bool*>(property->property_type);
            const char* text = *value ? "True" : "False";

            if (property->can_edit)
                GuiToggle(default_value_bounds, text, value);

            else
                GuiStatusBar(default_value_bounds, text);
        }

        // float
        else if (std::holds_alternative<float*>(property->property_type)) {
            float* value = get<float*>(property->property_type);

            if (property->can_edit)
                DrawFloatSpinner(
                    default_value_bounds,
                    value,
                    property);
            else
                GuiStatusBar(
                    default_value_bounds,
                    std::to_string(*value).c_str());
        }

        // Vec2
        else if (std::holds_alternative<Vector2*>(property->property_type)) {
            default_box = false;
            default_label = false;

            // Draw big box (two rows)
            GuiGroupBox(
                { item_bounds.x,
                    item_bounds.y,
                    item_bounds.width,
                    item_bounds.height + list_items_height + list_items_spacing },
                NULL);

            // Get property name (to append .x and .y)
            std::string property_name = (property->name);
            Vector2* value = get<Vector2*>(property->property_type);

            // Draw x label
            GuiLabel(
                { item_inner_bounds.x, item_inner_bounds.y, default_label_width, item_inner_bounds.height },
                (property_name + ".x").c_str());

            // Draw x value
            if (property->can_edit)
                DrawFloatSpinner(
                    default_value_bounds,
                    &value->x,
                    property);
            else
                GuiStatusBar(
                    default_value_bounds,
                    std::to_string(value->x).c_str());

            // Add spacing
            item_bounds.y
                += list_items_height + list_items_spacing;
            item_inner_bounds.y = item_bounds.y + default_border_width + default_text_padding;

            // Draw y label
            GuiLabel(
                { item_inner_bounds.x, item_inner_bounds.y, default_label_width, item_inner_bounds.height },
                (property_name + ".y").c_str());

            // Draw y value
            if (property->can_edit)
                DrawFloatSpinner(
                    { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                    &value->y,
                    property);
            else
                GuiStatusBar(
                    { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                    std::to_string(value->y).c_str());
        }

        if (default_box)
            GuiGroupBox(item_bounds, NULL);

        if (default_label) {
            GuiLabel(
                { item_inner_bounds.x, item_inner_bounds.y, default_label_width, item_inner_bounds.height },
                property->name);
        }

        item_bounds.y += list_items_height + list_items_spacing;
    }

    return 0;
}

void DrawIntSpinner(Rectangle bounds, int* pointer, DebugProperty* properties)
{
    bool editing = CheckCollisionPointRec(GetMousePosition(), bounds);

    if (editing) {
        if (IsKeyPressed(KEY_MINUS))
            *pointer *= -1;

        int scroll_dir = GetMouseWheelMoveV().y;

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            scroll_dir *= 10;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            scroll_dir *= 100;
        }
        *pointer += scroll_dir;
    }

    int min_val = static_cast<int>(properties->min_val);
    int max_val = static_cast<int>(properties->max_val);

    GuiSpinner(
        bounds,
        NULL,
        pointer,
        min_val, max_val,
        editing);
}

void DrawFloatSpinner(Rectangle bounds, float* pointer, DebugProperty* properties)
{
    // TODO - change to most rescent version of RayGui for float spinners

    bool editing = CheckCollisionPointRec(GetMousePosition(), bounds);

    if (editing) {
        if (IsKeyPressed(KEY_MINUS))
            *pointer *= -1;

        float scroll_dir = GetMouseWheelMoveV().y * properties->scroll_scale;

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            scroll_dir *= 20;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            scroll_dir *= 100;
        }
        *pointer += scroll_dir;
        *pointer = Clamp(*pointer, properties->min_val, properties->max_val);
    }

    GuiStatusBar(bounds, std::to_string(*pointer).c_str());
}
