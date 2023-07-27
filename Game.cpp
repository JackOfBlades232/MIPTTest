#include "Engine.h"
#include "Params.h"
#include "Levels.h"
#include "Vec.h"
#include "Rect.h"
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


// @TODO: separate out rendering, input and gameplay logic
// @TODO: create game state struct
// @TODO: create header for settings and constants, and a header for levels
// @TODO: refac

// @TODO: make tile maps for levels and read from ascii, then from file, refac level data
// @TODO: keep static only platrform info, and allocate real platforms yourself
// @TODO: refactor globals and unify C and C++ style structs where needed where needed
// @TODO: add static asserts to all constants
// @TODO: fix .h to <c..> in includes

// @TODO: collectables
// @TODO: font rendering & score
// @TODO: game over/win UI

#define BYTES_PER_PIXEL 4
#define IMAGE_PITCH     SCREEN_WIDTH * BYTES_PER_PIXEL

#define SECTOR_PIX_SIZE MIN(SCREEN_WIDTH/GEOM_WIDTH, SCREEN_HEIGHT/GEOM_HEIGHT)

struct player_t {
    rect_t  rect;
    vec2f_t velocity;
    vec2f_t acceleration;
    // @TODO: refac?
    bool movement_button_pressed;

    u32     num_sectors;
    rect_t  sectors[4];

    // @TODO: factor out to game state
    bool won, died;

    // @TEST
    f32 time_after_side_wall_collision;
};

static f32 fixed_dt      = 0;
static u32 current_level = 0;

static player_t player;

static vec2f_t locate_player_spawn_in_geom(const static_geom_t *geom)
{
    const f32 in_square_offset_x = (1.-PLAYER_SIZE)/2;
    const f32 in_square_offset_y = 1.-PLAYER_SIZE;

    for (u32 y = 0; y < GEOM_HEIGHT; y++) 
        for (u32 x = 0; x < GEOM_WIDTH; x++) {
            if ((*geom)[y][x] == '^')
                return vec2f_t(x+in_square_offset_x, y+in_square_offset_y);
        }

    ASSERTF(0, "Assertion failed: There is no player spawn point in level %d\n", current_level);
    return vec2f_t();
}

static void init_player()
{
    player.rect.size = vec2f_t(PLAYER_SIZE, PLAYER_SIZE);
}

static void increment_level()
{
    current_level++;
    if (current_level >= LEVEL_CNT)
        current_level -= LEVEL_CNT;
}

static void reset_player()
{
    player.rect.pos = locate_player_spawn_in_geom(&level_geometries[current_level]);
    player.velocity = vec2f_t(PLAYER_INIT_VELOCITY_X, PLAYER_INIT_VELOCITY_Y);
    player.acceleration = vec2f_t(0, PLAYER_GRAVITY);

    player.movement_button_pressed = false;

    player.num_sectors = 0;

    player.won = false;
    player.died = false;

    player.time_after_side_wall_collision = SIDE_WALL_COLLISION_TIMEOUT+EPSILON;
}

static void reset_moving_platforms()
{
    moving_platform_arr_t cur_arr = level_moving_platforms[current_level];
    for (u32 i = 0; i < cur_arr.num_platforms; i++) {
        moving_platform_t *platform = &cur_arr.platforms[i];
        platform->rect.pos = platform->init_pos;
        platform->goal_idx = 0;
    }
}

static void reset_level()
{
    fixed_dt = 0;
    reset_player();
    reset_moving_platforms();
}

// initialize game data in this function
void initialize()
{
    current_level = 0;
    init_player();

    reset_level();
}

// Optimization to avoid checking all static sector collisions
// Relies on the fact that the player is smaller than the sector
static void player_collect_intersecting_sectors()
{
    player.num_sectors = 0;

    vec2f_t half_size = player.rect.size/2;
    vec2f_t player_center = player.rect.pos + half_size;

    f32 player_sector_x = floor(player_center.x);
    f32 player_sector_y = floor(player_center.y);
    player.sectors[player.num_sectors++] = rect_t(player_sector_x, player_sector_y, 1, 1); 

    player_center.x -= player_sector_x;
    player_center.y -= player_sector_y;

    // @TODO: less/greater or non-strict?
    if (player_sector_x > 0 && player_center.x < half_size.x)
        player.sectors[player.num_sectors++] = rect_t(player_sector_x-1, player_sector_y, 1, 1); 
    else if (player_sector_x < GEOM_WIDTH-1 && player_center.x > 1-half_size.x)
        player.sectors[player.num_sectors++] = rect_t(player_sector_x+1, player_sector_y, 1, 1); 

    if (player_sector_y > 0 && player_center.y < half_size.y)
        player.sectors[player.num_sectors++] = rect_t(player_sector_x, player_sector_y-1, 1, 1); 
    else if (player_sector_y < GEOM_HEIGHT-1 && player_center.y > 1-half_size.y)
        player.sectors[player.num_sectors++] = rect_t(player_sector_x, player_sector_y+1, 1, 1); 

    if (player.num_sectors == 3) {
        f32 last_sector_x = player.sectors[1].pos.x;
        f32 last_sector_y = player.sectors[2].pos.y;
        player.sectors[player.num_sectors++] = rect_t(last_sector_x, last_sector_y, 1, 1);
    }
}

static void resolve_player_to_static_rect_collision(rect_t *s_rect)
{
    if (player.velocity.is_zero() || !rects_are_intersecting(&player.rect, s_rect))
        return;

    // @TODO: avoid all the recalculations of half-size and center
    vec2f_t half_size = player.rect.size/2;
    vec2f_t player_center = player.rect.pos + half_size;
    vec2f_t player_dir = player.velocity.normalized();

    // Rect with a padding of player_size/2 on all sides
    rect_t ext_s_rect(s_rect->pos - half_size, s_rect->size + player.rect.size);
    
    f32 tmin;
    vec2f_t normal;
    bool intersected = intersect_ray_with_rect(player_center, player_dir, &ext_s_rect, &tmin, &normal);

    // we are supposed to only check the sectors that the player intersects with
    ASSERT(intersected);
    
    player.rect.pos += player_dir*tmin + normal*EPSILON; 
    player.velocity = reflect_vec(-player.velocity, normal);

    // @TEST:
    if (FZERO(normal.x))
        player.velocity.y = SGN(normal.y) * VERT_BOUNCE_VELOCITY;
    else
        player.time_after_side_wall_collision = 0;
}

static void resolve_player_collisions()
{
    const static_geom_t *geom = &level_geometries[current_level];

    for (u32 i = 0; i < player.num_sectors; i++) {
        u32 x = player.sectors[i].pos.x;
        u32 y = player.sectors[i].pos.y;

        switch ((*geom)[y][x]) {
            case '#':
                resolve_player_to_static_rect_collision(&player.sectors[i]);
                break;
            case '*':
                player.died = true;
                break;
            case '>':
                player.won = true;
                break;
            default:
                break;
        }
    }

    // @TODO: factor out
    moving_platform_arr_t cur_arr = level_moving_platforms[current_level];
    for (u32 i = 0; i < cur_arr.num_platforms; i++)
        resolve_player_to_static_rect_collision(&cur_arr.platforms[i].rect);
}

static void tick_player_movement()
{
    player.velocity += player.acceleration * fixed_dt;

    // @HACK
    f32 lateral_speed = player.velocity.x;
    if (!player.movement_button_pressed && !FZERO(lateral_speed)) {
        f32 d_speed = MIN(ABS(lateral_speed), PLAYER_LATERAL_DRAG * fixed_dt) * SGN(lateral_speed);
        player.velocity.x -= d_speed;
    }

    player.rect.pos += player.velocity * fixed_dt;
}

static void tick_moving_platforms_movement()
{
    moving_platform_arr_t cur_arr = level_moving_platforms[current_level];
    for (u32 i = 0; i < cur_arr.num_platforms; i++) {
        moving_platform_t *platform = &cur_arr.platforms[i];
        
        vec2f_t dest = platform->goal_positions[platform->goal_idx];
        vec2f_t to_dest = dest - platform->rect.pos;
        f32 dist = to_dest.mag();
        vec2f_t dir = to_dest.normalized();

        f32 offset = MIN(dist, platform->speed * fixed_dt);
        platform->rect.pos += dir * offset;

        if ((dest - platform->rect.pos).is_zero()) {
            platform->goal_idx++;
            if (platform->goal_idx >= platform->num_goals)
                platform->goal_idx -= platform->num_goals;
        }
    }
}

static void tick_physics(f32 dt)
{
    fixed_dt += dt;

    if (fixed_dt < PHYSICS_UPDATE_INTERVAL)
        return;

    // @TEST
    if (player.time_after_side_wall_collision < SIDE_WALL_COLLISION_TIMEOUT)
        player.time_after_side_wall_collision += fixed_dt;

    tick_player_movement();
    tick_moving_platforms_movement();

    player_collect_intersecting_sectors();
    resolve_player_collisions();

    fixed_dt = 0;
}

static void process_input()
{
    bool l_pressed = is_key_pressed(VK_LEFT);
    bool r_pressed = is_key_pressed(VK_RIGHT);

    // @TEST
    if (player.time_after_side_wall_collision < SIDE_WALL_COLLISION_TIMEOUT)
        return;

    // @TODO: refac and optimize boolean setting
    if (l_pressed && !r_pressed) {
        player.velocity.x = -PLAYER_LATERAL_SPEED;
        player.movement_button_pressed = true;
    } else if (r_pressed && !l_pressed) {
        player.velocity.x = PLAYER_LATERAL_SPEED;
        player.movement_button_pressed = true;
    } else
        player.movement_button_pressed = false;
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(f32 dt)
{
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    printf("%6.5f s per frame\n", dt);

    process_input();
    tick_physics(dt);

    if (player.won) {
        increment_level();
        reset_level();
    } else if (player.died)
        reset_level();
}

static void draw_rect(rect_t *r, u32 color)
{
    u32 screen_pos_x = r->pos.x*SECTOR_PIX_SIZE;
    u32 screen_pos_y = r->pos.y*SECTOR_PIX_SIZE;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    u32 screen_size_x = r->size.x*SECTOR_PIX_SIZE;
    u32 screen_size_y = r->size.y*SECTOR_PIX_SIZE;

    // @TODO: factor apart?
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

static void draw_static_geom()
{
    const static_geom_t *geom = &level_geometries[current_level];
    rect_t sector_rect(0, 0, 1, 1);

    for (; sector_rect.pos.y < GEOM_HEIGHT; sector_rect.pos.y++)
        for (sector_rect.pos.x = 0; sector_rect.pos.x < GEOM_WIDTH; sector_rect.pos.x++) {
            u32 x = sector_rect.pos.x;
            u32 y = sector_rect.pos.y;
            u32 color;

            switch ((*geom)[y][x]) {
                case '#':
                    color = SURFACE_COLOR;
                    break;
                case '*':
                    color = DANGER_COLOR;
                    break;
                case '>':
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
    moving_platform_arr_t cur_arr = level_moving_platforms[current_level];
    for (u32 i = 0; i < cur_arr.num_platforms; i++)
        draw_rect(&cur_arr.platforms[i].rect, SURFACE_COLOR);
}

static inline void draw_player()
{
    draw_rect(&player.rect, PLAYER_COLOR);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u32));
    draw_static_geom();
    draw_moving_platforms();
    // @TODO: draw collectables
    draw_player();
}

// free game data in this function
void finalize()
{
}

