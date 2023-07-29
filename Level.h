#pragma once
#include "Params.h"
#include "Vec.h"
#include "Geom.h"
#include <stdlib.h>

/// Param types for static config ///

typedef char static_geom_map_t[GEOM_HEIGHT][GEOM_WIDTH+1];

struct moving_platform_params_t {
    rect_t init_rect;
    f32 speed;

    u32 num_goals;
    const vec2f_t *goal_positions;
    
    char geom_type_glyph;

    inline moving_platform_params_t(
            vec2f_t init_pos, vec2f_t size, f32 speed, 
            const vec2f_t *goals, u32 num_goals, char geom_type_glyph
            ) : 
        init_rect(init_pos, size), speed(speed), num_goals(num_goals),
        goal_positions(goals), geom_type_glyph(geom_type_glyph) {}
};

struct level_params_t { 
    const static_geom_map_t geom_map;
    const moving_platform_params_t *platform_params; 
    const u32 num_platforms; 
};

/// Structs for use in-engine ///

struct moving_platform_t {
    rect_t rect;
    u32 goal_idx;

    const moving_platform_params_t *params;

    inline moving_platform_t(const moving_platform_params_t *params) :
        rect(params->init_rect), goal_idx(0), params(params) {}

    inline f32 speed()          { return params->speed; }
    inline u32 num_goals()      { return params->num_goals; }

    inline vec2f_t cur_goal()   { return params->goal_positions[goal_idx]; }
    inline void inc_goal_idx()  { if (++goal_idx >= params->num_goals) goal_idx -= params->num_goals; }

    inline char glyph()         { return params->geom_type_glyph; }

    inline void reset()         { rect = params->init_rect; goal_idx = 0; }
};

struct collectable_t {
    circle_t shape;
    bool was_collected;

    // For up-down movement
    vec2f_t velocity;
    vec2f_t acceleration;
    f32 seconds_since_changed_direction;

    vec2f_t init_pos;
    vec2f_t init_velocity;
    vec2f_t init_acc;

    inline collectable_t(vec2f_t pos, f32 rad, vec2f_t velocity, vec2f_t acceleration) :
        shape(pos, rad), was_collected(false), velocity(velocity), 
        acceleration(acceleration), seconds_since_changed_direction(0),
        init_pos(pos), init_velocity(velocity), init_acc(acceleration) {}

    inline void reset() 
    { 
        shape.center = init_pos; 
        velocity = init_velocity; 
        acceleration = init_acc; 

        was_collected = false; 
        seconds_since_changed_direction = 0; 
    }
};

struct level_t {
    moving_platform_t *platforms; 
    collectable_t *collectables; 

    const level_params_t *params;
    u32 num_collectables;

    inline level_t() : platforms(nullptr), collectables(nullptr), params(nullptr) {}
    inline level_t(u32 index, const level_params_t *params_arr) :
        platforms(nullptr), collectables(nullptr), params(&params_arr[index]), num_collectables(0) {}

    inline const static_geom_map_t *geom_map()     { return &params->geom_map; }
    inline u32 num_platforms()                     { return params->num_platforms; }
    inline moving_platform_t *get_platform(u32 i)  { return &platforms[i]; }
    inline collectable_t *get_collectable(u32 i)   { return &collectables[i]; }

    void init();
    void cleanup();
    void reset();
};
