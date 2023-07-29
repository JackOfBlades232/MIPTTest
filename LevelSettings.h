#pragma once
#include "Level.h"

static const vec2f_t goal_positions11[] = { vec2f_t(19, 21.5), vec2f_t(19, 12.5) };
static const vec2f_t goal_positions12[] = { vec2f_t(26, 6.5), vec2f_t(26, 13.5) };
static const vec2f_t goal_positions13[] = { vec2f_t(1.5, 12), vec2f_t(8.5, 12) };

static const moving_platform_params_t level1_platform_params[] = {
    moving_platform_params_t(vec2f_t(19, 19), vec2f_t(5, 1), 3, goal_positions11, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(26, 9), vec2f_t(4, 1), 3, goal_positions12, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(8, 12), vec2f_t(4, 1), 3, goal_positions13, 2, SURFACE_GLYPH)
};

static const vec2f_t goal_positions21[] = { vec2f_t(2, 14), vec2f_t(2, 17.5) };
static const vec2f_t goal_positions22[] = { vec2f_t(2.5, 21), vec2f_t(9.5, 21) };
static const vec2f_t goal_positions23[] = { vec2f_t(12, 11), vec2f_t(16, 11) };
static const vec2f_t goal_positions24[] = { vec2f_t(12, 6), vec2f_t(16, 6) };
static const vec2f_t goal_positions25[] = { vec2f_t(27, 12), vec2f_t(23, 12) };

static const moving_platform_params_t level2_platform_params[] = {
    moving_platform_params_t(vec2f_t(2, 16), vec2f_t(3, 1), 3, goal_positions21, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(2.5, 21), vec2f_t(4, 1), 3, goal_positions22, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(16, 11), vec2f_t(2, 1), 4, goal_positions23, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(12, 6), vec2f_t(2, 1), 4, goal_positions24, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(25, 12), vec2f_t(3, 1), 3, goal_positions25, 2, DANGER_GLYPH)
};

static const vec2f_t goal_positions31[] = { vec2f_t(15, 8), vec2f_t(15, 13) };
static const vec2f_t goal_positions32[] = { vec2f_t(27, 8.5), vec2f_t(27, 17.5) };
static const vec2f_t goal_positions33[] = { vec2f_t(8, 1), vec2f_t(21, 1) };

static const moving_platform_params_t level3_platform_params[] = {
    moving_platform_params_t(vec2f_t(15, 10), vec2f_t(2, 5), 4, goal_positions31, 2, DANGER_GLYPH),
    moving_platform_params_t(vec2f_t(27, 17.5), vec2f_t(3, 1), 3, goal_positions32, 2, SURFACE_GLYPH),
    moving_platform_params_t(vec2f_t(8, 1), vec2f_t(3, 1), 4, goal_positions33, 2, EXIT_GLYPH),
};

const level_params_t all_levels_params[LEVEL_CNT] = {
    {
        {
            "################################", 
            "#                            >>#",
            "#                            >>#",
            "#                  *    *    >>#",
            "#                  * $  *    >>#",
            "#          #####################",
            "#        ##                    #",
            "#      ##                      #",
            "#    ##                        #",
            "#                              #",
            "###                   $        #",
            "#            ############      #",
            "#            #                 #",
            "#            #                 #",
            "#************#                 #",
            "#                        #######",
            "#                        #######",
            "#                              #",
            "#                              #",
            "#             $                #",
            "#   >        ####              #",
            "#   #############              #",
            "# ^ #############            $ #",
            "################################",
        },
        level1_platform_params, sizeof(level1_platform_params)/sizeof(*level1_platform_params)
    },
    {
        {
            "################################", 
            "#         *                    #",
            "#         *                    #",
            "#         *                    #",
            "# ^  >    *          $         #",
            "######    *         #####      #",
            "#         *          *         #",
            "#         *          *         #",
            "#         *          *         #",
            "#         *       ###*         #",
            "#     ####*          *         #",
            "#***      *          *         #",
            "#      $$ *          *         #",
            "#         *          *         #",
            "#         *#######   *         #",
            "#         *    #     *         #",
            "#         *    #     *         #",
            "#              #   ##*         #",
            "#                    *       >>#",
            "#                    *       >>#",
            "#                 ###*       >>#",
            "#                   $*       >>#",
            "#              ######*    ######",
            "#*************************######",
        },
        level2_platform_params, sizeof(level2_platform_params)/sizeof(*level2_platform_params)
    },
    {
        {
            "****#####**************#####****", 
            "*                              *",
            "*                              *",
            "*   $                      $   *",
            "*  ##      ##      ##      ##  *",
            "*                              *",
            "*                              *",
            "*#########################     *",
            "*                              *",
            "*                              *",
            "*              $$              *",
            "*                              *",
            "*                              *",
            "*          ####  ####          *",
            "*                    ##        *",
            "*      ##              ##      *",
            "*                              *",
            "*                              *",
            "*  ##                          *",
            "*                              *",
            "*      ##                      *",
            "*                              *",
            "*             ^>               *",
            "**********############**********",
        },
        level3_platform_params, sizeof(level3_platform_params)/sizeof(*level3_platform_params)
    }
};
