#include "Engine.h"
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

// @TODO: collisions with walls (DEBUG stage)

// @TODO: BUG: while cliping with great speed the player teleports (shall not be a gameplay problem)
// @TODO: add assertions (change clipping to assert, player shan't touch the edge)
// @TODO: remake char switches to enum
// @TODO: refactor globals where needed
// @TODO: go over u32/s32 choices, and change ij to xy where needed
// @TODO: add static asserts to all constants
// @TODO: fix .h to <c..> in includes

#define BYTES_PER_PIXEL 4
#define IMAGE_PITCH     SCREEN_WIDTH * BYTES_PER_PIXEL

#define GEOM_WIDTH      32
#define GEOM_HEIGHT     24
#define SECTOR_PIX_SIZE MIN(SCREEN_WIDTH/GEOM_WIDTH, SCREEN_HEIGHT/GEOM_HEIGHT)

typedef char static_geom_t[GEOM_HEIGHT][GEOM_WIDTH+1];

static static_geom_t level_geometries[] = { {
        "################################",
        "#                            >>#",
        "#                            >>#",
        "#                  *    *    >>#",
        "#                  *    *    >>#",
        "#          #####################",
        "#        ##                    #",
        "#      ##                      #",
        "#    ##                        #",
        "#                              #",
        "###                            #",
        "#            ############      #",
        "#            #                 #",
        "#            #                 #",
        "#************#                 #",
        "#                        #######",
        "#                        #######",
        "#                              #",
        "#                              #",
        "# ^                            #",
        "#            ####              #",
        "#   #############              #",
        "#   #############              #",
        "################################",
    }
};
// @TODO: refac
#define LEVEL_CNT sizeof(level_geometries)/sizeof(*level_geometries)

#define SURFACE_COLOR 0x90EE90
#define DANGER_COLOR  0xFC6A03
#define EXIT_COLOR    0x5C54A4
#define PLAYER_COLOR  0x8F00FF

#define PLAYER_SIZE              0.5
#define PLAYER_GRAVITY           8.0
#define PLAYER_BOUNCE_FACTOR     1.0
#define PLAYER_INIT_VELOCITY_X   0.0
#define PLAYER_INIT_VELOCITY_Y   -8.0
#define PLAYER_LATERAL_ACC       64.0
#define PLAYER_MAX_LATERAL_SPEED 8.0

typedef struct player_tag {
    rect_t  rect;
    vec2f_t velocity;
    vec2f_t acceleration;

    u32     num_sectors;
    rect_t  sectors[4];
} player_t;

// @TEST
#define PHYSICS_UPDATE_INTERVAL 1./60.
static f32 fixed_dt      = 0;
static u32 current_level = 0;

static player_t player;

static vec2f_t locate_player_spawn_in_geom(static_geom_t *geom)
{
    const f32 in_square_offset_x = (1.-PLAYER_SIZE)/2;
    const f32 in_square_offset_y = 1.-PLAYER_SIZE;

    for (u32 y = 0; y < GEOM_HEIGHT; y++) 
        for (u32 x = 0; x < GEOM_WIDTH; x++) {
            if ((*geom)[y][x] == '^')
                return vec2f_t(x+in_square_offset_x, y+in_square_offset_y);
        }

    // @TODO: log error
    return vec2f_t();
}

static void init_player()
{
    player.rect.pos = locate_player_spawn_in_geom(&level_geometries[current_level]);
    player.rect.size = vec2f_t(PLAYER_SIZE, PLAYER_SIZE);
    player.velocity = vec2f_t(PLAYER_INIT_VELOCITY_X, PLAYER_INIT_VELOCITY_Y);
    player.acceleration = vec2f_t(0, PLAYER_GRAVITY);
}

// initialize game data in this function
void initialize()
{
    current_level = 0;
    fixed_dt = 0;

    // @TODO: create level init phase
    init_player();
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

static void resolve_player_to_surface_sector_collision(rect_t *s_rect)
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
    // @TODO: assert intersected=true, we already checked the sectors
    
    player.rect.pos += player_dir*tmin + normal*EPSILON; 
    player.velocity = reflect_vec(-player.velocity, normal) * PLAYER_BOUNCE_FACTOR;
}

static void tick_physics(f32 dt)
{
    fixed_dt += dt;

    if (fixed_dt < PHYSICS_UPDATE_INTERVAL)
        return;

    player.velocity += player.acceleration * fixed_dt;
    player.velocity.x = CLIP(player.velocity.x, -PLAYER_MAX_LATERAL_SPEED, PLAYER_MAX_LATERAL_SPEED);

    player.rect.pos += player.velocity * fixed_dt;

    player_collect_intersecting_sectors();
    for (u32 i = 0; i < player.num_sectors; i++) {
        // @TODO: create some tilemap with sector info
        u32 x = player.sectors[i].pos.x;
        u32 y = player.sectors[i].pos.y;

        // @TEST
        char t = level_geometries[current_level][y][x];
        if (t == '#')
            resolve_player_to_surface_sector_collision(&player.sectors[i]);
    }

    fixed_dt = 0;
}

static void process_input()
{
    bool l_pressed = is_key_pressed(VK_LEFT);
    bool r_pressed = is_key_pressed(VK_RIGHT);
    if (l_pressed && !r_pressed)
        player.acceleration.x = -PLAYER_LATERAL_ACC;
    else if (r_pressed && !l_pressed)
        player.acceleration.x = PLAYER_LATERAL_ACC;
    else
        player.acceleration.x = 0;
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
}

static void draw_rect(rect_t *r, u32 color)
{
    u32 screen_pos_x = r->pos.x*SECTOR_PIX_SIZE;
    u32 screen_pos_y = r->pos.y*SECTOR_PIX_SIZE;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    u32 screen_size_x = r->size.x*SECTOR_PIX_SIZE;
    u32 screen_size_y = r->size.y*SECTOR_PIX_SIZE;
    // @TODO: assert correct size

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

    for (u32 y = screen_pos_y; y < screen_pos_y+screen_size_y; y++) {
        u32 *pixel = buffer[y] + screen_pos_x;
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++)
            *(pixel++) = color;
    }
}

static void draw_static_geom()
{
    static_geom_t *geom;
    geom = &level_geometries[current_level];
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

            //@TEST
            /*
            for (u32 i = 0; i < player.num_sectors; i++) {
                if (FEQ(x, player.sectors[i].pos.x) && FEQ(y, player.sectors[i].pos.y)) {
                    color = 0xFF00FF;
                    break;
                } 
            }
            */

            if (color) draw_rect(&sector_rect, color);
        }
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
    // @TODO: draw moving parts
    draw_player();
}

// free game data in this function
void finalize()
{
}

