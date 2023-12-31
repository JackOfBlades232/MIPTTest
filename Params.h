#pragma once
#include "Engine.h"
#include "Utils.h"

#define LEVEL_CNT 3

// In square sectors 
#define GEOM_WIDTH   32
#define GEOM_HEIGHT  24

#define PHYSICS_UPDATE_INTERVAL 1./60.

#define SURFACE_GLYPH     '#'
#define DANGER_GLYPH      '*'
#define EXIT_GLYPH        '>'
#define SPAWN_GLYPH       '^'
#define COLLECTABLE_GLYPH '$'

#define SURFACE_COLOR      0x90EE90
#define DANGER_COLOR       0xFC6A03
#define EXIT_COLOR         0x6A5ACD
#define PLAYER_COLOR       0x8F00FF
#define COLLECTABLE_COLOR  0xFFFF00
#define HUD_FONT_COLOR     0xFFFFFF
#define WIN_UI_BG_COLOR    0x90EE90
#define WIN_UI_FONT_COLOR  0x8F00FF

#define PLAYER_SIZE_X            0.4
#define PLAYER_SIZE_Y            0.4
#define PLAYER_GRAVITY           64.0
#define PLAYER_INIT_VELOCITY_X   0.0
#define PLAYER_INIT_VELOCITY_Y   -22.0
#define PLAYER_LATERAL_SPEED     8.0
#define PLAYER_LATERAL_DRAG      32.0

#define PLAYER_VERT_BOUNCE_VELOCITY        21.0
#define PLAYER_SIDE_WALL_COLLISION_TIMEOUT 0.17

#define COLLECTABLE_RAD          0.3
#define COLLECTABLE_AMPLITUDE    0.3
#define COLLECTABLE_PERIOD       1.2
#define COLLECTABLES_CAP         32 

#define UI_CHAR_SIZE_X     1
#define UI_CHAR_SIZE_Y     1

#define SCORE_PLACK_DIGITS 3
#define SCORE_PLACK_X      GEOM_WIDTH - SCORE_PLACK_DIGITS*UI_CHAR_SIZE_X - 1
#define SCORE_PLACK_Y      1

#define TIMER_X            1
#define TIMER_Y            1

#define WIN_UI_CHAR_SIZE_X   1
#define WIN_UI_CHAR_SIZE_Y   1
#define WIN_SCREEN_BG_SIZE_X 28*WIN_UI_CHAR_SIZE_X
#define WIN_SCREEN_BG_SIZE_Y 16*WIN_UI_CHAR_SIZE_Y    
#define WIN_SCREEN_BG_X      (GEOM_WIDTH - WIN_SCREEN_BG_SIZE_X)/2
#define WIN_SCREEN_BG_Y      (GEOM_HEIGHT - WIN_SCREEN_BG_SIZE_Y)/2

static const char win_screen_line1[] = "YOU'VE WON!";
static const char win_screen_line2[] = "PRESS ENTER TO RESTART";
#define WIN_SCREEN_LINE1_LEN (sizeof(win_screen_line1)/sizeof(*win_screen_line1) - 1)
#define WIN_SCREEN_LINE2_LEN (sizeof(win_screen_line2)/sizeof(*win_screen_line2) - 1)
#define WIN_SCREEN_LINE1_OFF 6*WIN_UI_CHAR_SIZE_Y
#define WIN_SCREEN_LINE2_OFF 8*WIN_UI_CHAR_SIZE_Y


// Not tweakable
#define COLLECTABLE_ACCELERATION 2*COLLECTABLE_AMPLITUDE / (COLLECTABLE_PERIOD*COLLECTABLE_PERIOD)
#define COLLECTABLE_INIT_SPEED   COLLECTABLE_ACCELERATION*COLLECTABLE_PERIOD/2

// Timer format : "mm:ss"
#define TIMER_FMT_LEN     5
#define TIMER_MAX_SECONDS 99*60+99

// So that lines are centered
#define WIN_SCREEN_LINE1_X WIN_SCREEN_BG_X + (WIN_SCREEN_BG_SIZE_X - WIN_SCREEN_LINE1_LEN * WIN_UI_CHAR_SIZE_X)/2
#define WIN_SCREEN_LINE2_X WIN_SCREEN_BG_X + (WIN_SCREEN_BG_SIZE_X - WIN_SCREEN_LINE2_LEN * WIN_UI_CHAR_SIZE_X)/2

// Resolution-specific defines, depend on params & engine
#define BYTES_PER_PIXEL 4
#define IMAGE_PITCH     SCREEN_WIDTH * BYTES_PER_PIXEL

#define SECTOR_PIX_SIZE MIN(SCREEN_WIDTH/GEOM_WIDTH, SCREEN_HEIGHT/GEOM_HEIGHT)

#define SCREEN_LEFT_PADDING (SCREEN_WIDTH - GEOM_WIDTH*SECTOR_PIX_SIZE)/2
#define SCREEN_TOP_PADDING  (SCREEN_HEIGHT - GEOM_HEIGHT*SECTOR_PIX_SIZE)/2

STATIC_ASSERT(LEVEL_CNT > 0);

STATIC_ASSERT(GEOM_WIDTH > 0 && GEOM_WIDTH <= MIN(SCREEN_WIDTH, SCREEN_HEIGHT));
STATIC_ASSERT(GEOM_HEIGHT > 0 && GEOM_HEIGHT <= MIN(SCREEN_HEIGHT, SCREEN_HEIGHT));
STATIC_ASSERT(PHYSICS_UPDATE_INTERVAL >= 0.0);

// Player needs to be smaller than geometry tile for the collisions to work properly
STATIC_ASSERT(PLAYER_SIZE_X > 0.0 && PLAYER_SIZE_X < 1.0);
STATIC_ASSERT(PLAYER_SIZE_X > 0.0 && PLAYER_SIZE_X < 1.0);

STATIC_ASSERT(PLAYER_GRAVITY > 0.0);
STATIC_ASSERT(PLAYER_LATERAL_SPEED > 0.0);
STATIC_ASSERT(PLAYER_LATERAL_DRAG >= 0.0);
STATIC_ASSERT(PLAYER_VERT_BOUNCE_VELOCITY >= 0.0);

// Without the timeout the physics do not behave correctly (when pressing into the wall)
STATIC_ASSERT(PLAYER_SIDE_WALL_COLLISION_TIMEOUT > PHYSICS_UPDATE_INTERVAL);

STATIC_ASSERT(COLLECTABLE_RAD > 0.0 && COLLECTABLE_RAD < 0.5);
STATIC_ASSERT(COLLECTABLE_AMPLITUDE > 0.0);
STATIC_ASSERT(COLLECTABLE_PERIOD > 0.0);
STATIC_ASSERT(COLLECTABLES_CAP > 0);

STATIC_ASSERT(UI_CHAR_SIZE_X > 0.0 && UI_CHAR_SIZE_Y > 0.0);

STATIC_ASSERT(SCORE_PLACK_DIGITS > 0 && SCORE_PLACK_DIGITS < 11);
STATIC_ASSERT(SCORE_PLACK_X >= 0 && SCORE_PLACK_X <= GEOM_WIDTH - SCORE_PLACK_DIGITS*UI_CHAR_SIZE_X);
STATIC_ASSERT(SCORE_PLACK_Y >= 0 && SCORE_PLACK_Y <= GEOM_HEIGHT - UI_CHAR_SIZE_Y);

STATIC_ASSERT(TIMER_X >= 0 && TIMER_X <= GEOM_WIDTH - TIMER_FMT_LEN*UI_CHAR_SIZE_X);
STATIC_ASSERT(TIMER_Y >= 0 && TIMER_Y <= GEOM_HEIGHT - UI_CHAR_SIZE_Y);

STATIC_ASSERT(TIMER_FMT_LEN == 5);
STATIC_ASSERT(TIMER_MAX_SECONDS == 99*60+99);

STATIC_ASSERT(WIN_UI_CHAR_SIZE_X > 0.0 && WIN_UI_CHAR_SIZE_Y > 0.0);
STATIC_ASSERT(WIN_SCREEN_BG_SIZE_X >= WIN_UI_CHAR_SIZE_X && WIN_SCREEN_BG_SIZE_Y >= 2*WIN_UI_CHAR_SIZE_Y);
STATIC_ASSERT(WIN_SCREEN_BG_X >= 0 && WIN_SCREEN_BG_X <= GEOM_WIDTH-WIN_SCREEN_BG_SIZE_X);
STATIC_ASSERT(WIN_SCREEN_BG_Y >= 0 && WIN_SCREEN_BG_Y <= GEOM_WIDTH-WIN_SCREEN_BG_SIZE_Y);

STATIC_ASSERT(WIN_SCREEN_LINE1_LEN > 0 && WIN_SCREEN_LINE1_LEN <= WIN_SCREEN_BG_SIZE_X / WIN_UI_CHAR_SIZE_X);
STATIC_ASSERT(WIN_SCREEN_LINE2_LEN > 0 && WIN_SCREEN_LINE2_LEN <= WIN_SCREEN_BG_SIZE_X / WIN_UI_CHAR_SIZE_X);
STATIC_ASSERT(WIN_SCREEN_LINE1_OFF >= 0 && WIN_SCREEN_LINE1_OFF < WIN_SCREEN_BG_SIZE_Y / WIN_UI_CHAR_SIZE_Y);
STATIC_ASSERT(WIN_SCREEN_LINE2_OFF >= 0 && WIN_SCREEN_LINE2_OFF < WIN_SCREEN_BG_SIZE_Y / WIN_UI_CHAR_SIZE_Y);
STATIC_ASSERT(WIN_SCREEN_LINE1_OFF <= WIN_SCREEN_LINE2_OFF-WIN_UI_CHAR_SIZE_Y);
