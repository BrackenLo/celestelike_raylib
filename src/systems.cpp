#include "systems.hpp"

#include "components.hpp"
#include "helper.hpp"
#include "player_resources.hpp"
#include "raylib.h"
#include "raymath.h"
#include "resources.hpp"
#include "tools.hpp"
#include <cmath>

namespace celestelike {

void render::render(entt::registry& reg, float dt)
{
    auto v_camera = reg.view<GameCamera>();
    auto v_sprite = reg.view<Pos, Render, Sprite>();

    entt::entity camera_entity = v_camera.front();
    Camera2D camera = camera_entity == entt::null ? default_camera() : v_camera.get<GameCamera>(camera_entity);

    BeginDrawing();

    Color clear_color = reg.ctx().contains<ClearColor>() ? reg.ctx().get<ClearColor>() : RAYWHITE;
    ClearBackground(clear_color);

    BeginMode2D(camera);

    for (const entt::entity entity : v_sprite) {

        Render& render = v_sprite.get<Render>(entity);

        if (!render.visible)
            continue;

        Pos& pos = v_sprite.get<Pos>(entity);
        Sprite& sprite = v_sprite.get<Sprite>(entity);

        draw_rect(pos, sprite);
    }

    EndMode2D();

    EndDrawing();
}

void physics::apply_velocity_collision(entt::registry& reg, float dt)
{
    auto v_actor = reg.view<Actor, Pos, CollisionBounds, Velocity>();
    auto v_solid = reg.view<Solid, Pos, CollisionBounds>();

    for (entt::entity entity : v_actor) {
        Pos& pos = v_actor.get<Pos>(entity);
        const CollisionBounds& bounds = v_actor.get<CollisionBounds>(entity);
        Velocity& vel = v_actor.get<Velocity>(entity);

        //----------------------------------------------

        int left_nudge = 0;
        int right_nudge = 0;

        pos.x += vel.x * dt;

        for (entt::entity solid_entity : v_solid) {
            const Pos& solid_pos = v_solid.get<Pos>(solid_entity);
            const CollisionBounds& solid_bounds = v_solid.get<CollisionBounds>(solid_entity);

            if (check_collision(pos, bounds, solid_pos, solid_bounds)) {
                int depth = get_collision_depth_x(pos, bounds, solid_pos, solid_bounds);

                if (depth) {
                    if (depth > 0)
                        right_nudge = std::max(right_nudge, depth);
                    else
                        left_nudge = std::min(left_nudge, depth);
                }
            }
        }

        bool hit_left = left_nudge;
        bool hit_right = right_nudge;

        if (hit_left != hit_right) {
            // on_wall = true;
            pos.x += left_nudge + right_nudge;
            vel.x = 0;
        } else {
            // TODO - check squish (check if both true)
        }

        //----------------------------------------------

        int up_nudge = 0;
        int down_nudge = 0;

        pos.y += vel.y * dt;

        for (entt::entity solid_entity : v_solid) {
            const Pos& solid_pos = v_solid.get<Pos>(solid_entity);
            const CollisionBounds& solid_bounds = v_solid.get<CollisionBounds>(solid_entity);

            if (check_collision(pos, bounds, solid_pos, solid_bounds)) {

                int depth = get_collision_depth_y(pos, bounds, solid_pos, solid_bounds);

                if (depth) {
                    if (depth > 0)
                        down_nudge = std::max(down_nudge, depth);
                    else
                        up_nudge = std::min(up_nudge, depth);
                }
            }
        }

        bool hit_top = up_nudge;
        bool hit_bottom = down_nudge;

        if (hit_top != hit_bottom) {
            // if (hit_top)
            //     grounded = true;
            // else
            //     on_ceiling = true;
            vel.y = 0;
            pos.y += up_nudge + down_nudge;
        } else {
            // TODO - check squish (check if both true)
        }
    }
}

void physics::check_on_walls(entt::registry& reg, float dt)
{
    auto v_actor = reg.view<Actor, Pos, CollisionBounds>();
    auto v_solid = reg.view<Solid, Pos, CollisionBounds>();

    auto v_on_ground = reg.view<OnGround>();
    auto v_on_ceiling = reg.view<OnCeiling>();

    for (entt::entity actor : v_actor) {
        bool ground_hit = false;
        bool ceiling_hit = false;

        const Pos& actor_pos = v_actor.get<Pos>(actor);
        const Bounds& actor_bounds = v_actor.get<CollisionBounds>(actor);

        const Pos actor_check_pos = { actor_pos.x, actor_pos.y - 2 };
        const Bounds actor_check_bounds = {
            actor_bounds.half_width, actor_bounds.half_height + 4
        };

        for (entt::entity solid : v_solid) {
            const Pos& solid_pos = v_solid.get<Pos>(solid);
            const Bounds& solid_bounds = v_solid.get<CollisionBounds>(solid);

            if (check_collision(
                    actor_check_pos, actor_check_bounds,
                    solid_pos, solid_bounds) //
            ) {
                int depth = physics::get_collision_depth_y(
                    actor_check_pos, actor_check_bounds,
                    solid_pos, solid_bounds);

                if (depth < 0)
                    ground_hit = true;
                else
                    ceiling_hit = true;

                if (ground_hit && ceiling_hit)
                    break;
            }
        }

        //----------------------------------------------

        if (ground_hit) {
            if (v_on_ground.contains(actor)) {
                OnGround& on_ground = v_on_ground.get<OnGround>(actor);
                on_ground.just_started = false;

            } else {
                reg.emplace<OnGround>(actor, true);
            }

        } else {
            if (v_on_ground.contains(actor))
                reg.erase<OnGround>(actor);
        }

        if (ceiling_hit) {
            if (v_on_ceiling.contains(actor)) {
                OnCeiling& on_ceiling = v_on_ceiling.get<OnCeiling>(actor);
                on_ceiling.just_started = false;

            } else {
                reg.emplace<OnCeiling>(actor, true);
            }

        } else {
            if (v_on_ceiling.contains(actor))
                reg.erase<OnCeiling>(actor);
        }
    }
}

void update::pos_lerp(entt::registry& reg, float dt)
{
    auto view = reg.view<Pos, PosLerp>();

    for (entt::entity entity : view) {
        Pos& pos = reg.get<Pos>(entity);
        const PosLerp& lerp = reg.get<PosLerp>(entity);

        pos.x = Lerp(pos.x, lerp.target.x, lerp.speed_x * dt);
        pos.y = Lerp(pos.y, lerp.target.y, lerp.speed_y * dt);
    }
}

void player::init_systems(entt::registry& reg)
{
    reg.on_destroy<OnGround>().connect<&left_ground>(); // NOTE - triggers before component removal
}

// Update all player inputs buttons
void player::update_input(entt::registry& reg, float dt)
{
    PlayerInput& input = reg.ctx().get<PlayerInput>();
    FixedTimestep& fixed = reg.ctx().get<FixedTimestep>();

    bool right_down = are_keys_down(input.key_right);
    bool left_down = are_keys_down(input.key_left);
    input.x_dir = right_down - left_down;

    bool up_down = are_keys_down(input.key_up);
    bool down_down = are_keys_down(input.key_down);
    input.y_dir = up_down - down_down;

    input.jump_held = are_keys_down(input.key_jump);
    if (are_keys_pressed(input.key_jump)) {
        input.jump_pressed = true;
        input.time_jump_pressed = fixed.elapsed;
    }

    input.ability_1_pressed = input.ability_1_pressed || are_keys_pressed(input.key_ability_1);
    input.ability_2_pressed = input.ability_2_pressed || are_keys_pressed(input.key_ability_2);
    input.ability_3_pressed = input.ability_3_pressed || are_keys_pressed(input.key_ability_3);

    input.ability_1_down = are_keys_down(input.key_ability_1);
    input.ability_2_down = are_keys_down(input.key_ability_2);
}

void player::examine_collisions(entt::registry& reg, float dt)
{
    auto v_player = reg.view<Player, Velocity>();
    auto v_jump = reg.view<Jump>();

    auto v_on_ground = reg.view<OnGround>();
    auto v_on_ceiling = reg.view<OnCeiling>();

    for (entt::entity player : v_player) {
        Velocity& velocity = v_player.get<Velocity>(player);

        // Player is on ceiling
        if (v_on_ceiling.contains(player))
            velocity.y = std::max(0, velocity.y);

        // Player is on ground
        if (v_on_ground.contains(player)) {

            // Player can jump
            if (v_jump.contains(player)) {
                Jump& jump = v_jump.get<Jump>(player);

                jump.coyote_usable = true;
                jump.buffered_jump_usable = true;
                jump.ended_early = false;
            }
        }
    }
}

void player::left_ground(entt::registry& reg, entt::entity player)
{
    const FixedTimestep& fixed_time = reg.ctx().get<FixedTimestep>();

    auto v_jump = reg.view<Jump>();
    if (v_jump.contains(player)) {
        Jump& jump = v_jump.get<Jump>(player);

        jump.time_left_ground = fixed_time.elapsed;
    }
}

// Apply player horizontal velocity
void player::handle_walk(entt::registry& reg, float dt)
{
    const PlayerInput& input = reg.ctx().get<PlayerInput>();
    auto v_players = reg.view<Player, Pos, Velocity, WalkSpeed>();

    for (const entt::entity entity : v_players) {
        Pos& pos = v_players.get<Pos>(entity);
        Velocity& velocity = v_players.get<Velocity>(entity);
        WalkSpeed& walk_speed = v_players.get<WalkSpeed>(entity);

        int target_velocity = input.x_dir * walk_speed.max_walk_speed;
        int speed;

        if (input.x_dir == 0)
            speed = walk_speed.deaccel;
        else {
            int target_sign = std::copysign(1, target_velocity);
            int current_sign = std::copysign(1, velocity.x);

            speed = target_sign == current_sign ? walk_speed.accel : walk_speed.deaccel;
        }

        velocity.x = istep(velocity.x, target_velocity, speed * dt);
    }
}

bool has_buffered_jump(const Jump& jump, const PlayerInput& input, const FixedTimestep& fixed)
{
    return jump.buffered_jump_usable && fixed.elapsed < input.time_jump_pressed + jump.jump_buffer;
}

bool can_coyote_jump(const Jump& jump, bool grounded, const FixedTimestep& fixed)
{
    return jump.coyote_usable && !grounded && fixed.elapsed < jump.time_left_ground + jump.coyote_time;
}

void player::handle_jump(entt::registry& reg, float dt)
{
    PlayerInput& input = reg.ctx().get<PlayerInput>();
    FixedTimestep& fixed = reg.ctx().get<FixedTimestep>();

    auto v_player = reg.view<Player, Velocity, Jump>();
    auto v_on_ground = reg.view<OnGround>();

    for (entt::entity entity : v_player) {
        Velocity& velocity = v_player.get<Velocity>(entity);
        Jump& jump = v_player.get<Jump>(entity);

        bool grounded = v_on_ground.contains(entity);

        // Check if variable jump height applicable
        if (!jump.ended_early && !grounded && !input.jump_held && velocity.y < 0)
            jump.ended_early = true;

        // Check if jump pressed or was pressed in time
        if (!input.jump_pressed && !has_buffered_jump(jump, input, fixed))
            continue;

        // Check if we can jump
        if (grounded || can_coyote_jump(jump, grounded, fixed)) {
            velocity.y = jump.impulse;

            jump.ended_early = false;
            input.time_jump_pressed = 0;
            jump.buffered_jump_usable = false;
            jump.coyote_usable = false;
        }
    }
}

void player::handle_gravity(entt::registry& reg, float dt)
{
    auto v_player = reg.view<Player, Velocity, Gravity>();
    auto v_jump = reg.view<Jump>();
    auto v_grounded = reg.view<OnGround>();

    for (entt::entity player : v_player) {
        Velocity& velocity = v_player.get<Velocity>(player);
        const Gravity& gravity = v_player.get<Gravity>(player);

        // On ground
        if (v_grounded.contains(player) && velocity.y >= 0.0f) {
            velocity.y = 100; // TODO | TEST - Grounding force

        } else {
            int fall_speed = gravity.fall_speed;

            if (v_jump.contains(player)) {
                Jump& jump = v_jump.get<Jump>(player);

                if (jump.ended_early && velocity.y < 0)
                    fall_speed *= jump.fall_multiplier;
            }

            velocity.y = istep(velocity.y, gravity.max_fall_speed, fall_speed * dt);
        }
    }
}

void player::reset_values(entt::registry& reg, float dt)
{
    PlayerInput& input = reg.ctx().get<PlayerInput>();

    input.jump_pressed = false;
    input.ability_1_pressed = false;
    input.ability_2_pressed = false;
    input.ability_3_pressed = false;
}

void camera::camera_follow(entt::registry& reg, float dt)
{
    auto v_camera = reg.view<GameCamera, PosLerp>();
    auto v_target = reg.view<CameraTarget, Pos>();

    for (entt::entity camera_entity : v_camera) {
        PosLerp& lerp = v_camera.get<PosLerp>(camera_entity);

        for (entt::entity target_entity : v_target) {
            Pos& pos = v_target.get<Pos>(target_entity);
            lerp.target = pos;

            break;
        }
    }
}

void camera::camera_update(entt::registry& reg, float dt)
{
    auto v_camera = reg.view<GameCamera>();
    auto v_pos = reg.view<Pos>();

    bool screen_resize = IsWindowResized();

    for (entt::entity entity : v_camera) {
        Camera2D& camera = v_camera.get<GameCamera>(entity);

        if (v_pos.contains(entity)) {
            const Pos& pos = v_pos.get<Pos>(entity);
            camera.target = { static_cast<float>(pos.x), static_cast<float>(pos.y) };
        }

        if (screen_resize)
            camera.offset = {
                GetScreenWidth() / 2.0f,
                GetScreenHeight() / 2.0f
            };
    }
}

}
