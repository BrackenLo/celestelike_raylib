#include "player_avian.h"

AvianPlayerInner::AvianPlayerInner(Player* outer)
    : PlayerInner(outer)
{
    player_type = PlayerType::Avian;

    outer->half_width = 20;
    outer->half_height = 36;

    outer->player_color = GREEN;

    accel = 2400.0f;
    deaccel = 2000.0f;

    max_velocity_x = 450.0f;
    max_fall_speed = 700.0f;

    jump_height = -60.0f; // Height without holding space
    jump_time_to_peak = 0.3f;
    jump_time_to_descent = 0.2f;
    variable_jump_height = -200.0f;
    total_jumps = 2;

    update_jump_variables();
}
