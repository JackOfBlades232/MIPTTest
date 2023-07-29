#include "Engine.h"
#include "Level.h"
#include "Ui.h"
#include "Draw.h"
#include "Geom.h"
#include "Vec.h"
#include "Params.h"
#include "LevelSettings.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

// @TODO: play test
// @TODO: final code lookthrough
// @TODO: reset resolution and test on windows

/// Global structures ///

enum game_fsm_state_t {
    fsm_game,
    fsm_win_scren
};

struct game_state_t {
    game_fsm_state_t state;

    f32  fixed_dt;
    f32  seconds_since_level_start;

    bool movement_controls_locked;
    bool movement_button_pressed;

    u32  cur_level_idx;

    u32 score;
    u32 prev_score;
    u32 max_score;

    bool level_completed;
    bool player_died;

    // Optimization to only make the win screnn UI "reactive" (in this case -- static)
    bool win_screen_dirty;

    inline game_state_t() : state(fsm_game), fixed_dt(0), seconds_since_level_start(0),
        movement_controls_locked(false), movement_button_pressed(false), cur_level_idx(0),
        score(0), prev_score(0), max_score(0), level_completed(false), player_died(false),
        win_screen_dirty(true) {}

    inline game_state_t(u32 max_score) : state(fsm_game), fixed_dt(0),
        seconds_since_level_start(0), movement_controls_locked(false), 
        movement_button_pressed(false), cur_level_idx(0), score(0), 
        prev_score(0), max_score(max_score), level_completed(false), 
        player_died(false), win_screen_dirty(true) {}

    void reset()
    {
        fixed_dt = 0;
        seconds_since_level_start = 0;
        score = prev_score;
        movement_controls_locked = false;
        movement_button_pressed = false;
        level_completed = false;
        player_died = false;
    }

    void full_reset()
    {
        state = fsm_game;
        prev_score = 0;
        cur_level_idx = 0;
        win_screen_dirty = true;
        reset();
    }
};

struct player_box_t {
    rect_t   rect;
    vec2f_t  velocity;
    vec2f_t  acceleration;

    vec2f_t  half_size;

    u32      num_sectors;
    rect_t   sectors[4];
    f32      seconds_after_side_wall_collision;

    inline player_box_t() : rect(), velocity(), acceleration(), half_size(),
        num_sectors(0), sectors(), seconds_after_side_wall_collision(0) {}
    inline player_box_t(vec2f_t size) :
        rect(vec2f_t(), size), velocity(), acceleration(),
        num_sectors(0), sectors(), seconds_after_side_wall_collision(0)
    {
        half_size = rect.size/2;
    }

    void reset(vec2f_t spawn_pos)
    {
        rect.pos = spawn_pos;
        velocity = vec2f_t(PLAYER_INIT_VELOCITY_X, PLAYER_INIT_VELOCITY_Y);
        acceleration = vec2f_t(0, PLAYER_GRAVITY);

        num_sectors = 0;

        seconds_after_side_wall_collision = PLAYER_SIDE_WALL_COLLISION_TIMEOUT+EPSILON;
    }
};

/// Global game state ///
static game_state_t  game_state; static player_box_t  player_box;
static level_t       current_level;

/// Initialization and level switching functions ///

static void reset_level();
static vec2f_t locate_player_spawn();

void initialize()
{
    game_state = game_state_t(ui_get_max_displayabe_score());
    player_box = player_box_t(vec2f_t(PLAYER_SIZE_X, PLAYER_SIZE_Y));

    current_level = level_t(game_state.cur_level_idx, all_levels_params);
    current_level.init();

    game_state.full_reset();
    reset_level();
}

static void reset_level()
{
    game_state.reset();
    player_box.reset(locate_player_spawn());
    current_level.reset();
}

static vec2f_t locate_player_spawn()
{
    const static_geom_map_t *geom_map = current_level.geom_map();

    f32 in_square_offset_x = (1.-player_box.rect.size.x)/2;
    f32 in_square_offset_y = 1.-player_box.rect.size.y;

    for (u32 y = 0; y < GEOM_HEIGHT; y++) 
        for (u32 x = 0; x < GEOM_WIDTH; x++) {
            if ((*geom_map)[y][x] == SPAWN_GLYPH)
                return vec2f_t(x+in_square_offset_x, y+in_square_offset_y);
        }

    ASSERTF(0, "Assertion failed: There is no player spawn point in level %d\n", game_state.cur_level_idx);
    return vec2f_t();
}

static void switch_level()
{
    game_state.prev_score = game_state.score;

    current_level.cleanup();
    current_level = level_t(game_state.cur_level_idx, all_levels_params);
    current_level.init();

    reset_level();
}

static void increment_level()
{
    game_state.cur_level_idx++;
    if (game_state.cur_level_idx >= LEVEL_CNT) {
        current_level.cleanup();
        game_state.state = fsm_win_scren;
    } else
        switch_level();
}

/// ACT PHASE ///

static void process_input();
static void tick_physics(f32 dt);

void act(f32 dt)
{
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    printf("%6.5f s per frame\n", dt);

    switch (game_state.state) {
        case fsm_game:
            {
                game_state.seconds_since_level_start += dt;

                process_input();
                tick_physics(dt);

                if (game_state.level_completed)
                    increment_level();
                else if (game_state.player_died)
                    reset_level();
            }
            break;

        case fsm_win_scren:
            {
                if (is_key_pressed(VK_RETURN)) {
                    game_state.full_reset();
                    switch_level();
                }
            }
            break;
    }

}

static void process_input()
{
    if (game_state.movement_controls_locked)
        return;

    bool l_pressed = is_key_pressed(VK_LEFT);
    bool r_pressed = is_key_pressed(VK_RIGHT);

    game_state.movement_button_pressed = false;

    if (l_pressed && !r_pressed) {
        player_box.velocity.x = -PLAYER_LATERAL_SPEED;
        game_state.movement_button_pressed = true;
    } else if (r_pressed && !l_pressed) {
        player_box.velocity.x = PLAYER_LATERAL_SPEED;
        game_state.movement_button_pressed = true;
    }
}

static void tick_player_movement();
static void tick_moving_platforms_movement();
static void tick_collectables_movement();
static void resolve_player_collisions();

static void tick_physics(f32 dt)
{
    game_state.fixed_dt += dt;

    if (game_state.fixed_dt < PHYSICS_UPDATE_INTERVAL)
        return;

    tick_player_movement();
    tick_moving_platforms_movement();
    tick_collectables_movement();

    resolve_player_collisions();

    game_state.fixed_dt = 0;
}

/// Movement ticking for all kinametic objects ///

static void tick_player_movement()
{
    if (player_box.seconds_after_side_wall_collision < PLAYER_SIDE_WALL_COLLISION_TIMEOUT)
        player_box.seconds_after_side_wall_collision += game_state.fixed_dt;
    else if (game_state.movement_controls_locked)
        game_state.movement_controls_locked = false;

    player_box.velocity += player_box.acceleration * game_state.fixed_dt;

    // Apply lateral drag if player is not holding any buttons
    f32 lateral_speed = player_box.velocity.x;
    if (!game_state.movement_button_pressed && !FZERO(lateral_speed)) {
        f32 d_speed = MIN(ABS(lateral_speed), PLAYER_LATERAL_DRAG * game_state.fixed_dt) * SGN(lateral_speed);
        player_box.velocity.x -= d_speed;
    }

    player_box.rect.pos += player_box.velocity * game_state.fixed_dt;
}

static void tick_moving_platforms_movement()
{
    for (u32 i = 0; i < current_level.num_platforms(); i++) {
        moving_platform_t *platform = current_level.get_platform(i);
        if ((platform->cur_goal() - platform->rect.pos).is_zero())
            platform->inc_goal_idx();
        
        vec2f_t dest = platform->cur_goal();
        vec2f_t to_dest = dest - platform->rect.pos;
        f32 dist = to_dest.mag();
        vec2f_t dir = to_dest.normalized();

        f32 offset = MIN(dist, platform->speed() * game_state.fixed_dt);
        platform->rect.pos += dir * offset;

        if ((dest - platform->rect.pos).is_zero())
            platform->inc_goal_idx();
    }
}

static void tick_collectables_movement()
{
    for (u32 i = 0; i < current_level.num_collectables; i++) {
        collectable_t *collectable = current_level.get_collectable(i);

        collectable->seconds_since_changed_direction += game_state.fixed_dt;
        if (collectable->seconds_since_changed_direction >= COLLECTABLE_PERIOD) {
            f32 excess_time = collectable->seconds_since_changed_direction - COLLECTABLE_PERIOD;
            f32 prev_time = game_state.fixed_dt - excess_time;

            collectable->velocity += collectable->acceleration * prev_time;
            collectable->shape.center += collectable->velocity * prev_time;

            collectable->acceleration = -collectable->acceleration;
            
            collectable->velocity += collectable->acceleration * excess_time;
            collectable->shape.center += collectable->velocity * excess_time;

            collectable->seconds_since_changed_direction = excess_time;
        } else {
            collectable->velocity += collectable->acceleration * game_state.fixed_dt;
            collectable->shape.center += collectable->velocity * game_state.fixed_dt;
        }
    }
}

/// Player box collision physics ///

static void player_collect_intersecting_sectors();
static void resolve_player_to_static_rect_collision(rect_t *s_rect, char geom_type_glyph);

static void resolve_player_collisions()
{
    const static_geom_map_t *geom_map = current_level.geom_map();

    // Calculate which sectors the player intersects with (up to 4)
    player_collect_intersecting_sectors();

    // Resolve collisions with static sectors
    for (u32 i = 0; i < player_box.num_sectors; i++) {
        u32 x = player_box.sectors[i].pos.x;
        u32 y = player_box.sectors[i].pos.y;
        char glyph = (*geom_map)[y][x];

        resolve_player_to_static_rect_collision(&player_box.sectors[i], glyph);
    }

    // Resolve collisions with moving platforms
    for (u32 i = 0; i < current_level.num_platforms(); i++) {
        moving_platform_t *platform = current_level.get_platform(i);
        resolve_player_to_static_rect_collision(&platform->rect, platform->glyph());
    }

    // Resolve collisions with collectables
    for (u32 i = 0; i < current_level.num_collectables; i++) {
        collectable_t *collectable = current_level.get_collectable(i);
        if (collectable->was_collected)
            continue;

        if (circle_and_rect_are_intersecting(&collectable->shape, &player_box.rect)) {
            collectable->was_collected = true;
            if (game_state.score < game_state.max_score)
                game_state.score++;
        }
    }
}

// Optimization to avoid checking all static sector collisions
// Relies on the fact that the player is smaller than the sector
static void player_collect_intersecting_sectors()
{
    player_box.num_sectors = 0;

    vec2f_t player_center = player_box.rect.pos + player_box.half_size;

    f32 player_sector_x = floor(player_center.x);
    f32 player_sector_y = floor(player_center.y);
    player_box.sectors[player_box.num_sectors++] = rect_t(player_sector_x, player_sector_y, 1, 1); 

    player_center.x -= player_sector_x;
    player_center.y -= player_sector_y;

    if (player_sector_x > 0 && player_center.x <= player_box.half_size.x)
        player_box.sectors[player_box.num_sectors++] = rect_t(player_sector_x-1, player_sector_y, 1, 1); 
    else if (player_sector_x < GEOM_WIDTH-1 && player_center.x >= 1-player_box.half_size.x)
        player_box.sectors[player_box.num_sectors++] = rect_t(player_sector_x+1, player_sector_y, 1, 1); 

    if (player_sector_y > 0 && player_center.y <= player_box.half_size.y)
        player_box.sectors[player_box.num_sectors++] = rect_t(player_sector_x, player_sector_y-1, 1, 1); 
    else if (player_sector_y < GEOM_HEIGHT-1 && player_center.y >= 1-player_box.half_size.y)
        player_box.sectors[player_box.num_sectors++] = rect_t(player_sector_x, player_sector_y+1, 1, 1); 

    if (player_box.num_sectors == 3) {
        f32 last_sector_x = player_box.sectors[1].pos.x;
        f32 last_sector_y = player_box.sectors[2].pos.y;
        player_box.sectors[player_box.num_sectors++] = rect_t(last_sector_x, last_sector_y, 1, 1);
    }
}

static void resolve_player_to_static_rect_collision(rect_t *s_rect, char geom_type_glyph)
{
    if (!rects_are_intersecting(&player_box.rect, s_rect))
        return;

    switch (geom_type_glyph) {
        case SURFACE_GLYPH:
            // Continue with the function body
            break;
        case DANGER_GLYPH:
            game_state.player_died = true;
            return;
        case EXIT_GLYPH:
            game_state.level_completed = true;
            return;
        default:
            return;
    }

    if (player_box.velocity.is_zero())
        return;

    vec2f_t player_center = player_box.rect.pos + player_box.half_size;
    vec2f_t player_dir = player_box.velocity.normalized();

    // Rect with a padding of player_size/2 on all sides
    rect_t ext_s_rect(s_rect->pos - player_box.half_size, s_rect->size + player_box.rect.size);
    
    f32 tmin;
    vec2f_t normal;
    bool intersected = intersect_ray_with_rect(player_center, player_dir, &ext_s_rect, &tmin, &normal);
    if (!intersected)
        return;
    
    player_box.rect.pos += player_dir*tmin + normal*EPSILON; 
    player_box.velocity = reflect_vec(-player_box.velocity, normal);

    // Two hacks:
    // 1) In order for the game to be playable, the box must always
    //      bounce up with the same speed
    // 2) If the box collides with a vertical wall and the player_box is moving into it, 
    //      the box will be stuck to the wall (since player_box controls reset velocity)
    //      to avoid this, I disable player_box-controlled movement for some time after
    //      a side wall collision
    if (FZERO(normal.x)) // horiz wall/surface
        player_box.velocity.y = SGN(normal.y) * PLAYER_VERT_BOUNCE_VELOCITY;
    else { // vertical (side) wall/surface
        game_state.movement_controls_locked = true;
        player_box.seconds_after_side_wall_collision = 0;
    }
}

/// DRAW PHASE ///

static void draw_static_geom();
static void draw_moving_platforms();
static void draw_collectables();

static u32 get_glyph_color(char glyph);

void draw()
{
    switch (game_state.state) {
        case fsm_game:
            {
                memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u32));

                draw_static_geom();
                draw_moving_platforms();
                draw_collectables();
                draw_rect(&player_box.rect, PLAYER_COLOR);

                ui_draw_score(game_state.score);
                ui_draw_timer(game_state.seconds_since_level_start);
            }
            break;

        case fsm_win_scren:
            {
                if (game_state.win_screen_dirty) {
                    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u32));
                    ui_draw_win_screen();

                    game_state.win_screen_dirty = false;
                }
            }
            break;
    }
}

static void draw_static_geom()
{
    const static_geom_map_t *geom_map = current_level.geom_map();
    rect_t sector_rect(0, 0, 1, 1);

    for (; sector_rect.pos.y < GEOM_HEIGHT; sector_rect.pos.y++)
        for (sector_rect.pos.x = 0; sector_rect.pos.x < GEOM_WIDTH; sector_rect.pos.x++) {
            u32 x = sector_rect.pos.x;
            u32 y = sector_rect.pos.y;
            u32 color = get_glyph_color((*geom_map)[y][x]);

            if (color) draw_rect(&sector_rect, color);
        }
}

static void draw_moving_platforms()
{
    for (u32 i = 0; i < current_level.num_platforms(); i++) {
        moving_platform_t *platform = current_level.get_platform(i);
        u32 color = get_glyph_color(platform->glyph());
        if (color) draw_rect(&platform->rect, color);
    }
}

static void draw_collectables()
{
    for (u32 i = 0; i < current_level.num_collectables; i++) {
        collectable_t *collectable = current_level.get_collectable(i);
        if (collectable->was_collected)
            continue;

        draw_circle(&collectable->shape, COLLECTABLE_COLOR);
    }
}

static u32 get_glyph_color(char glyph)
{
    switch (glyph) {
        case SURFACE_GLYPH:
            return SURFACE_COLOR;
        case DANGER_GLYPH:
            return DANGER_COLOR;
        case EXIT_GLYPH:
            return EXIT_COLOR;
        default:
            return 0;
    }
}

/// Game deinitialization ///

void finalize()
{
    current_level.cleanup();
}
