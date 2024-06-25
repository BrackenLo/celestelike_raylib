#include "ui.h"

#include "raygui.h"
#include "raylib.h"
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

        bool default_box = true;
        bool default_label = true;

        // int
        if (std::holds_alternative<int*>(property->property_type)) {

            bool editing = CheckCollisionPointRec(GetMousePosition(), item_bounds);

            GuiSpinner(
                { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                NULL,
                get<int*>(property->property_type),
                property->min_val, property->max_val,
                editing);

        }

        // bool
        else if (std::holds_alternative<bool*>(property->property_type)) {
            GuiLabel(
                { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                std::to_string(*get<bool*>(property->property_type)).c_str());
        }

        // float
        else if (std::holds_alternative<float*>(property->property_type)) {
            // can't use floats yet - cast to ints
            float* pointer = get<float*>(property->property_type);
            int val = static_cast<int>(*pointer);

            bool editing = CheckCollisionPointRec(GetMousePosition(), item_bounds);

            GuiSpinner(
                { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                NULL,
                &val,
                property->min_val, property->max_val,
                editing);

            *pointer = static_cast<float>(val);
        }

        // Vec2
        else if (std::holds_alternative<Vector2*>(property->property_type)) {
            default_box = false;
            default_label = false;

            GuiGroupBox(
                { item_bounds.x,
                    item_bounds.y,
                    item_bounds.width,
                    item_bounds.height + list_items_height + list_items_spacing },
                NULL);

            std::string property_name = (property->name);

            GuiLabel(
                { item_inner_bounds.x, item_inner_bounds.y, default_label_width, item_inner_bounds.height },
                (property_name + ".x").c_str());

            // can't use floats yet - cast to ints
            float* pointer_x = &get<Vector2*>(property->property_type)->x;
            int val_x = static_cast<int>(*pointer_x);

            bool editing_x = CheckCollisionPointRec(GetMousePosition(), item_bounds);

            GuiSpinner(
                { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                NULL,
                &val_x,
                property->min_val, property->max_val,
                editing_x);

            *pointer_x = static_cast<float>(val_x);

            // ---

            item_bounds.y += list_items_height + list_items_spacing;
            item_inner_bounds.y = item_bounds.y + default_border_width + default_text_padding;

            GuiLabel(
                { item_inner_bounds.x, item_inner_bounds.y, default_label_width, item_inner_bounds.height },
                (property_name + ".y").c_str());

            // can't use floats yet - cast to ints
            float* pointer_y = &get<Vector2*>(property->property_type)->y;
            int val_y = static_cast<int>(*pointer_y);

            bool editing_y = CheckCollisionPointRec(GetMousePosition(), item_bounds);

            GuiSpinner(
                { default_value_x, item_inner_bounds.y, default_value_width, item_inner_bounds.height },
                NULL,
                &val_y,
                property->min_val, property->max_val,
                editing_y);

            *pointer_y = static_cast<float>(val_y);
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
