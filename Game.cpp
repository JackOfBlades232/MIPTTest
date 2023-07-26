#include "Engine.h"
#include "Vec.h"
#include "Utils.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  schedule_quit_game() - quit game after act()

// @TODO: watch the lone coder vid to the end
// @TODO: rect struct & collisions
// @TODO: collisions with walls

// @TODO: BUG: while cliping with great speed the player teleports (shall not be a gameplay problem)
// @TODO: add assertions (change clipping to assert, player shan't touch the edge)
// @TODO: remake char switches to enum
// @TODO: refactor globals where needed
// @TODO: go over u32/s32 choices, and change ij to xy where needed
// @TODO: add static asserts to all constants

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
        "#                              #",
        "#            ####              #",
        "#   #############              #",
        "# ^ #############              #",
        "################################",
    }
};
// @TODO: refac
#define LEVEL_CNT sizeof(level_geometries)/sizeof(*level_geometries)

#define SURFACE_COLOR 0x90EE90
#define DANGER_COLOR  0xFC6A03
#define EXIT_COLOR    0x5C54A4
#define PLAYER_COLOR  0x8F00FF

#define PLAYER_SIZE            0.5
#define PLAYER_GRAVITY         8.0
#define PLAYER_BOUNCE_FACTOR   1.0
#define PLAYER_INIT_VELOCITY_X 0.0
#define PLAYER_INIT_VELOCITY_Y -8.0
#define PLAYER_LATERAL_SPEED   8.0

typedef struct player_tag {
    vec2f_t pos;
    vec2f_t velocity;
} player_t;

// @TEST
#define PHYSICS_UPDATE_INTERVAL      1./60.
static f32 fixed_dt      = 0;
static u32 current_level = 0;

static player_t player;

static vec2f_t locate_player_spawn_in_geom(static_geom_t *geom)
{
    const f32 in_square_offset_x = (1.-PLAYER_SIZE)/2;
    const f32 in_square_offset_y = 1.-PLAYER_SIZE;

    for (u32 i = 0; i < GEOM_HEIGHT; i++) 
        for (u32 j = 0; j < GEOM_WIDTH; j++) {
            if ((*geom)[i][j] == '^')
                return vec2f_t(j+in_square_offset_x, i+in_square_offset_y);
        }

    // @TODO: log error
    return vec2f_t();
}

static void init_player()
{
    player.pos = locate_player_spawn_in_geom(&level_geometries[current_level]);
    player.velocity = vec2f_t(PLAYER_INIT_VELOCITY_X, PLAYER_INIT_VELOCITY_Y);
}

// initialize game data in this function
void initialize()
{
    current_level = 0;
    fixed_dt = 0;

    // @TODO: create level init phase
    init_player();
}

static void tick_physics(f32 dt)
{
    fixed_dt += dt;

    if (fixed_dt > PHYSICS_UPDATE_INTERVAL) {
        player.velocity.y += PLAYER_GRAVITY * fixed_dt;
        player.pos += player.velocity * fixed_dt;

        fixed_dt = 0;
    }
}

static void process_input()
{
    bool l_pressed = is_key_pressed(VK_LEFT);
    bool r_pressed = is_key_pressed(VK_RIGHT);
    if (l_pressed && !r_pressed)
        player.velocity.x = -PLAYER_LATERAL_SPEED;
    else if (r_pressed && !l_pressed)
        player.velocity.x = PLAYER_LATERAL_SPEED;
    else
        player.velocity.x = 0;
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(f32 dt)
{
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    //printf("%6.5f s per frame\n", dt);

    process_input();
    tick_physics(dt);
}

static void draw_rect(f32 pos_x, f32 pos_y, f32 size_x, f32 size_y, u32 color)
{
    u32 screen_pos_x = pos_x*SECTOR_PIX_SIZE;
    u32 screen_pos_y = pos_y*SECTOR_PIX_SIZE;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    u32 screen_size_x = size_x*SECTOR_PIX_SIZE;
    u32 screen_size_y = size_y*SECTOR_PIX_SIZE;
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

static inline void draw_square(f32 pos_x, f32 pos_y, f32 size, u32 color)
{
    draw_rect(pos_x, pos_y, size, size, color);
}

static void draw_static_geom()
{
    static_geom_t *geom;
    geom = &level_geometries[current_level];

    for (u32 i = 0; i < GEOM_HEIGHT; i++) 
        for (u32 j = 0; j < GEOM_WIDTH; j++) {
            u32 color = SURFACE_COLOR;
            switch ((*geom)[i][j]) {
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

            if (color) draw_square(j, i, 1., color);
        }
}

static inline void draw_player()
{
    draw_square(player.pos.x, player.pos.y, PLAYER_SIZE, PLAYER_COLOR);
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

