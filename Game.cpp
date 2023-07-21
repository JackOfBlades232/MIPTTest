#include "Engine.h"
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

// @TODO: border clipping
// @TODO: gravity
// @TODO: input->lateral movement
// @TODO: collisions with walls

// @TODO: add assertions
// @TODO: add scaling and world coords
// @TODO: refactor globals where needed

typedef float     f32;
typedef double    f64;
typedef uint8_t   u8; 
typedef uint16_t  u16; 
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef struct vec2_tag { u32 c[2]; }  vec2_t;
typedef struct vec2f_tag { f32 c[2]; } vec2f_t;

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#define BYTES_PER_PIXEL 4
#define IMAGE_PITCH     SCREEN_WIDTH * BYTES_PER_PIXEL

#define SQUARE_SIZE     32
#define GEOM_WIDTH      SCREEN_WIDTH/SQUARE_SIZE
#define GEOM_HEIGHT     SCREEN_HEIGHT/SQUARE_SIZE

typedef char      static_geom_t[GEOM_HEIGHT][GEOM_WIDTH+1];

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

#define PLAYER_SIZE            16
#define PLAYER_BOUNCE_FACTOR   1.0
#define PLAYER_INIT_VELOCITY_X 0.0
#define PLAYER_INIT_VELOCITY_Y -128.0
#define PLAYER_LATERAL_SPEED   16.0

typedef struct player_tag {
    vec2_t pos;
    vec2f_t velocity;
} player_t;

static player_t player   = { 0 };
static u32 current_level = 0;

static vec2_t locate_player(static_geom_t *geom)
{
    const u32 in_square_offset_x = (SQUARE_SIZE-PLAYER_SIZE)/2;
    const u32 in_square_offset_y = SQUARE_SIZE-PLAYER_SIZE;

    // @TODO: might be done comptime
    vec2_t pos = { 0 };
    for (u32 i = 0; i < GEOM_HEIGHT; i++) 
        for (u32 j = 0; j < GEOM_WIDTH; j++) {
            if ((*geom)[i][j] == '^') {
                pos.c[0] = j*SQUARE_SIZE + in_square_offset_x;
                pos.c[1] = i*SQUARE_SIZE + in_square_offset_y;
                return pos;
            }
        }

    // @TODO: log error
    return pos;
}

// initialize game data in this function
void initialize()
{
    current_level = 0; // @TODO: remove one of them
    // @TODO: create level init phase
    player.pos = locate_player(&level_geometries[current_level]);
    player.velocity.c[0] = PLAYER_INIT_VELOCITY_X;
    player.velocity.c[1] = PLAYER_INIT_VELOCITY_Y;
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(f32 dt)
{
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    printf("%6.5f s per frame\n", dt);

    // @TEST
    player.pos.c[0] += player.velocity.c[0] * dt;
    player.pos.c[1] += player.velocity.c[1] * dt;
}

static void draw_rect(u32 pos_x, u32 pos_y, u32 size_x, u32 size_y, u32 color)
{
    // @TODO: clip/assert correct pos & size

    for (u32 y = pos_y; y < pos_y+size_y; y++) {
        u32 *pixel = buffer[y] + pos_x;
        for (u32 x = pos_x; x < pos_x+size_x; x++)
            *(pixel++) = color;
    }
}

static inline void draw_square(u32 pos_x, u32 pos_y, u32 size, u32 color)
{
    draw_rect(pos_x, pos_y, size, size, color);
}

static void draw_static_geom()
{
    static_geom_t *geom;
    geom = &level_geometries[current_level];
    // @TODO: speed up to rect drawing
    for (u32 i = 0; i < GEOM_HEIGHT; i++) 
        for (u32 j = 0; j < GEOM_WIDTH; j++) {
            u32 sq_y = i*SQUARE_SIZE;
            u32 sq_x = j*SQUARE_SIZE;

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

            if (color) draw_square(sq_x, sq_y, SQUARE_SIZE, color);
        }
}

static inline void draw_player()
{
    draw_square(player.pos.c[0], player.pos.c[1], PLAYER_SIZE, PLAYER_COLOR);
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

