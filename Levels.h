#pragma once
#include "Params.h"
#include "Vec.h"
#include "Rect.h"

#define LEVEL_CNT 1

typedef char static_geom_t[GEOM_HEIGHT][GEOM_WIDTH+1];
const static_geom_t level_geometries[LEVEL_CNT] = { {
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

// @TODO: refac somehow
struct moving_platform_t {
    rect_t rect;
    f32 speed;

    vec2f_t init_pos;
    u32 num_goals;
    u32 goal_idx;
    const vec2f_t *goal_positions;

    moving_platform_t(vec2f_t init_pos, vec2f_t size, f32 speed, const vec2f_t *goals, u32 num_goals) :
        rect(init_pos, size), speed(speed), init_pos(init_pos), 
        num_goals(num_goals), goal_idx(0), goal_positions(goals) {}
};

const vec2f_t goal_positions1[] = { vec2f_t(19, 21.5), vec2f_t(19, 12.5) };
const vec2f_t goal_positions2[] = { vec2f_t(26, 6.5), vec2f_t(26, 13.5) };
const vec2f_t goal_positions3[] = { vec2f_t(1.5, 12), vec2f_t(8.5, 12) };

moving_platform_t level1_platforms[] {
    moving_platform_t(vec2f_t(19, 19), vec2f_t(5, 1), 3, goal_positions1, 2),
    moving_platform_t(vec2f_t(26, 9), vec2f_t(4, 1), 3, goal_positions2, 2),
    moving_platform_t(vec2f_t(8, 12), vec2f_t(4, 1), 3, goal_positions3, 2)
};

struct moving_platform_arr_t {
    u32 num_platforms;
    moving_platform_t *platforms;
};

const moving_platform_arr_t level_moving_platforms[LEVEL_CNT] = {
    { 3, level1_platforms }
};
