#include "Engine.h"
#include "Vec.h"
#include "Rect.h"
#include "Level.h"
#include "Params.h"
#include "LevelSettings.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  schedule_quit_game() - quit game after act()


// @TODO: collectables
// @TODO: font rendering & score
// @TODO: win UI
// @TODO: game-design multiple levels

// @TODO: refac

// Optional
// - Saves
// - Particles&textures for collectables
// - Loading levels from files
// - Main menu with sidescrolling choice of levels (with static geom miniatures)

/// Engine-specific defines ///

#define BYTES_PER_PIXEL 4
#define IMAGE_PITCH     SCREEN_WIDTH * BYTES_PER_PIXEL

#define SECTOR_PIX_SIZE MIN(SCREEN_WIDTH/GEOM_WIDTH, SCREEN_HEIGHT/GEOM_HEIGHT)

/// Global structures ///

struct game_state_t {
    f32  fixed_dt;

    bool movement_button_pressed;

    u32  cur_level_idx;

    bool level_completed;
    bool player_died;

    inline game_state_t() : fixed_dt(0), movement_button_pressed(false), 
        cur_level_idx(0), level_completed(false), player_died(false) {}

    void reset()
    {
        fixed_dt = 0;
        movement_button_pressed = false;
        level_completed = false;
        player_died = false;
    }
};

struct player_box_t {
    rect_t   rect;
    vec2f_t  velocity;
    vec2f_t  acceleration;

    vec2f_t  half_size;

    u32      num_sectors;
    rect_t   sectors[4];
    f32      time_after_side_wall_collision;

    inline player_box_t() : rect(), velocity(), acceleration(), half_size(),
        num_sectors(0), sectors(), time_after_side_wall_collision(0) {}
    inline player_box_t(vec2f_t size) :
        rect(vec2f_t(), size), velocity(), acceleration(),
        num_sectors(0), sectors(), time_after_side_wall_collision(0)
    {
        half_size = rect.size/2;
    }

    void reset(vec2f_t spawn_pos)
    {
        rect.pos = spawn_pos;
        velocity = vec2f_t(PLAYER_INIT_VELOCITY_X, PLAYER_INIT_VELOCITY_Y);
        acceleration = vec2f_t(0, PLAYER_GRAVITY);

        num_sectors = 0;

        time_after_side_wall_collision = PLAYER_SIDE_WALL_COLLISION_TIMEOUT+EPSILON;
    }
};

/// Global game state ///
static game_state_t  game_state;
static player_box_t  player_box;
static level_t       current_level;

/// Initialization and level switching functions ///

static void reset_level();
static vec2f_t locate_player_spawn();

void initialize()
{
    player_box = player_box_t(vec2f_t(PLAYER_SIZE_X, PLAYER_SIZE_Y));
    current_level = level_t(game_state.cur_level_idx, all_levels_params);
    current_level.init();

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
    if (++game_state.cur_level_idx >= LEVEL_CNT)
        game_state.cur_level_idx -= LEVEL_CNT;

    current_level.cleanup();
    current_level = level_t(game_state.cur_level_idx, all_levels_params);
    current_level.init();

    reset_level();
}

/// ACT PHASE ///

static void process_input();
static void tick_physics(f32 dt);

void act(f32 dt)
{
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    printf("%6.5f s per frame\n", dt);

    process_input();
    tick_physics(dt);

    if (game_state.level_completed)
        switch_level();
    else if (game_state.player_died)
        reset_level();
}

static void process_input()
{
    bool l_pressed = is_key_pressed(VK_LEFT);
    bool r_pressed = is_key_pressed(VK_RIGHT);

    // @TODO: merge it somewhere with the logic from tick
    if (player_box.time_after_side_wall_collision < PLAYER_SIDE_WALL_COLLISION_TIMEOUT)
        return;

    game_state.movement_button_pressed = false;

    // @TODO: refac and optimize boolean setting
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
static void resolve_player_collisions();

static void tick_physics(f32 dt)
{
    game_state.fixed_dt += dt;

    if (game_state.fixed_dt < PHYSICS_UPDATE_INTERVAL)
        return;

    tick_player_movement();
    tick_moving_platforms_movement();

    resolve_player_collisions();

    game_state.fixed_dt = 0;
}

/// Movement ticking for all kinametic objects ///

static void tick_player_movement()
{
    // @TEST
    if (player_box.time_after_side_wall_collision < PLAYER_SIDE_WALL_COLLISION_TIMEOUT)
        player_box.time_after_side_wall_collision += game_state.fixed_dt;

    player_box.velocity += player_box.acceleration * game_state.fixed_dt;

    // @HACK
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

/// Player box collision physics ///

static void player_collect_intersecting_sectors();
static void resolve_player_to_static_rect_collision(rect_t *s_rect);

static void resolve_player_collisions()
{
    const static_geom_map_t *geom_map = current_level.geom_map();

    // Calculate which sectors the player intersects with (up to 4)
    player_collect_intersecting_sectors();

    // Resolve collisions with static sectors
    for (u32 i = 0; i < player_box.num_sectors; i++) {
        u32 x = player_box.sectors[i].pos.x;
        u32 y = player_box.sectors[i].pos.y;

        switch ((*geom_map)[y][x]) {
            case SURFACE_GLYPH:
                resolve_player_to_static_rect_collision(&player_box.sectors[i]);
                break;
            case DANGER_GLYPH:
                game_state.player_died = true;
                break;
            case EXIT_GLYPH:
                game_state.level_completed = true;
                break;
            default:
                break;
        }
    }

    // Resolve collisions with moving platforms
    for (u32 i = 0; i < current_level.num_platforms(); i++) {
        moving_platform_t *platform = current_level.get_platform(i);
        resolve_player_to_static_rect_collision(&platform->rect);
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

static void resolve_player_to_static_rect_collision(rect_t *s_rect)
{
    if (player_box.velocity.is_zero() || !rects_are_intersecting(&player_box.rect, s_rect))
        return;

    vec2f_t player_center = player_box.rect.pos + player_box.half_size;
    vec2f_t player_dir = player_box.velocity.normalized();

    // Rect with a padding of player_size/2 on all sides
    rect_t ext_s_rect(s_rect->pos - player_box.half_size, s_rect->size + player_box.rect.size);
    
    f32 tmin;
    vec2f_t normal;
    bool intersected = intersect_ray_with_rect(player_center, player_dir, &ext_s_rect, &tmin, &normal);

    // we are supposed to only check the sectors that the player_box intersects with
    ASSERT(intersected);
    
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
    else // vertical (side) wall/surface
        player_box.time_after_side_wall_collision = 0;
}

/// DRAW PHASE ///

static void draw_rect(rect_t *r, u32 color);

static void draw_static_geom();
static void draw_moving_platforms();
static void draw_collectables();

void draw()
{
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u32));
    draw_static_geom();
    draw_moving_platforms();
    // @TODO: draw collectables
    draw_rect(&player_box.rect, PLAYER_COLOR);
}

static void draw_static_geom()
{
    const static_geom_map_t *geom_map = current_level.geom_map();
    rect_t sector_rect(0, 0, 1, 1);

    for (; sector_rect.pos.y < GEOM_HEIGHT; sector_rect.pos.y++)
        for (sector_rect.pos.x = 0; sector_rect.pos.x < GEOM_WIDTH; sector_rect.pos.x++) {
            u32 x = sector_rect.pos.x;
            u32 y = sector_rect.pos.y;
            u32 color;

            switch ((*geom_map)[y][x]) {
                case SURFACE_GLYPH:
                    color = SURFACE_COLOR;
                    break;
                case DANGER_GLYPH:
                    color = DANGER_COLOR;
                    break;
                case EXIT_GLYPH:
                    color = EXIT_COLOR;
                    break;
                default:
                    color = 0;
                    break;
            }

            if (color) draw_rect(&sector_rect, color);
        }
}

static void draw_moving_platforms()
{
    for (u32 i = 0; i < current_level.num_platforms(); i++) {
        moving_platform_t *platform = current_level.get_platform(i);
        draw_rect(&platform->rect, SURFACE_COLOR);
    }
}

static void draw_rect(rect_t *r, u32 color)
{
    u32 screen_pos_x = r->pos.x*SECTOR_PIX_SIZE;
    u32 screen_pos_y = r->pos.y*SECTOR_PIX_SIZE;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    u32 screen_size_x = r->size.x*SECTOR_PIX_SIZE;
    u32 screen_size_y = r->size.y*SECTOR_PIX_SIZE;

    if (screen_pos_x < 0) {
        screen_size_x += screen_pos_x;
        screen_pos_x = 0;
    } else if (screen_pos_x > SCREEN_WIDTH-screen_size_x)
        screen_size_x = SCREEN_WIDTH-screen_pos_x;
    if (screen_pos_y < 0) {
        screen_size_y += screen_pos_y;
        screen_pos_y = 0;
    } else if (screen_pos_y > SCREEN_HEIGHT-screen_size_y)
        screen_size_y = SCREEN_HEIGHT-screen_pos_y;

    ASSERT(screen_pos_x >= 0 && screen_pos_x+screen_size_x <= SCREEN_WIDTH);
    ASSERT(screen_pos_y >= 0 && screen_pos_y+screen_size_y <= SCREEN_HEIGHT); 

    for (u32 y = screen_pos_y; y < screen_pos_y+screen_size_y; y++) {
        u32 *pixel = buffer[y] + screen_pos_x;
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++)
            *(pixel++) = color;
    }
}

static void draw_diamond(rect_t *rect, u32 color);

static void draw_collectables()
{
    for (u32 i = 0; i < current_level.num_collectables(); i++) {
        collectable_t *collectable = current_level.get_collectable(i);
        draw_diamond(&collectable->rect, COLLECTABLE_COLOR);
    }
}

static void draw_diamond(rect_t *rect, u32 color)
{
    // @TODO: implement diamond drawing
}

/// Game deinitialization ///

void finalize()
{
    current_level.cleanup();
}
