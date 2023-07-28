#pragma once
#include "Engine.h"
#include "Utils.h"

#define LEVEL_CNT 2

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
#define EXIT_COLOR         0x5C54A4
#define PLAYER_COLOR       0x8F00FF
#define COLLECTABLE_COLOR  0xFFFF00

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

STATIC_ASSERT(LEVEL_CNT > 0);

STATIC_ASSERT(GEOM_WIDTH > 0 && GEOM_WIDTH <= MIN(SCREEN_WIDTH, SCREEN_HEIGHT));
STATIC_ASSERT(GEOM_HEIGHT > 0 && GEOM_HEIGHT <= MIN(SCREEN_HEIGHT, SCREEN_HEIGHT));
STATIC_ASSERT(PHYSICS_UPDATE_INTERVAL >= 0.0);

// Player needs to be smaller than geometry tile for the collisions to work properly
STATIC_ASSERT(PLAYER_SIZE_X > 0.0 && PLAYER_SIZE_X < 1.0);
STATIC_ASSERT(PLAYER_SIZE_X > 0.0 && PLAYER_SIZE_X < 1.0);

STATIC_ASSERT(PLAYER_LATERAL_SPEED > 0.0);
STATIC_ASSERT(PLAYER_LATERAL_DRAG >= 0.0);
STATIC_ASSERT(PLAYER_VERT_BOUNCE_VELOCITY >= 0.0);

// Without the timeout the physics do not behave correctly (when pressing into the wall)
STATIC_ASSERT(PLAYER_SIDE_WALL_COLLISION_TIMEOUT > PHYSICS_UPDATE_INTERVAL);

STATIC_ASSERT(COLLECTABLE_RAD > 0.0 && COLLECTABLE_RAD < 0.5);
STATIC_ASSERT(COLLECTABLE_AMPLITUDE > 0.0);
STATIC_ASSERT(COLLECTABLE_PERIOD > 0.0);
STATIC_ASSERT(COLLECTABLES_CAP > 0);

// Not tweakable
#define COLLECTABLE_ACCELERATION 2*COLLECTABLE_AMPLITUDE / (COLLECTABLE_PERIOD*COLLECTABLE_PERIOD)
#define COLLECTABLE_INIT_SPEED   COLLECTABLE_ACCELERATION*COLLECTABLE_PERIOD/2
