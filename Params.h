#pragma once

#define LEVEL_CNT 2

#define GEOM_WIDTH   32
#define GEOM_HEIGHT  24

// @TEST
#define PHYSICS_UPDATE_INTERVAL 1./60.

#define SURFACE_GLYPH '#'
#define DANGER_GLYPH  '*'
#define EXIT_GLYPH    '>'
#define SPAWN_GLYPH   '^'

#define SURFACE_COLOR 0x90EE90
#define DANGER_COLOR  0xFC6A03
#define EXIT_COLOR    0x5C54A4
#define PLAYER_COLOR  0x8F00FF

#define PLAYER_SIZE_X            0.4
#define PLAYER_SIZE_Y            0.4
#define PLAYER_GRAVITY           64.0
#define PLAYER_INIT_VELOCITY_X   0.0
#define PLAYER_INIT_VELOCITY_Y   -22.0
#define PLAYER_LATERAL_SPEED     8.0
#define PLAYER_LATERAL_DRAG      32.0

// @TEST
#define VERT_BOUNCE_VELOCITY        21.0
#define SIDE_WALL_COLLISION_TIMEOUT 0.17
