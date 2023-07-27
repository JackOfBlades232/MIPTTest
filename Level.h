#pragma once
#include "Params.h"
#include "Vec.h"
#include "Rect.h"
#include <stdlib.h>

typedef char static_geom_map_t[GEOM_HEIGHT][GEOM_WIDTH+1];

struct moving_platform_params_t {
    rect_t init_rect;
    f32 speed;

    u32 num_goals;
    const vec2f_t *goal_positions;

    inline moving_platform_params_t(vec2f_t init_pos, vec2f_t size, f32 speed, const vec2f_t *goals, u32 num_goals) :
        init_rect(init_pos, size), speed(speed), num_goals(num_goals), goal_positions(goals) {}
};

struct level_params_t { 
    const static_geom_map_t geom_map;
    const u32 num_platforms; 
    const moving_platform_params_t *platform_params; 
};

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

    inline void reset()         { rect = params->init_rect; goal_idx = 0; }
};

struct level_t {
    moving_platform_t *platforms; 
    const level_params_t *params;

    inline level_t() : platforms(nullptr), params(nullptr) {}
    inline level_t(u32 index, const level_params_t *params_arr) :
        platforms(nullptr), params(&params_arr[index]) {}

    inline const static_geom_map_t *geom_map()     { return &params->geom_map; }
    inline u32 num_platforms()                     { return params->num_platforms; }
    inline moving_platform_t *get_platform(u32 i)  { return &platforms[i]; }

    void init();
    void cleanup();
    void reset();
};
