#pragma once
#include "Level.h"

// @TODO: expose only all_params and pack into a module?

static const vec2f_t goal_positions1[] = { vec2f_t(19, 21.5), vec2f_t(19, 12.5) };
static const vec2f_t goal_positions2[] = { vec2f_t(26, 6.5), vec2f_t(26, 13.5) };
static const vec2f_t goal_positions3[] = { vec2f_t(1.5, 12), vec2f_t(8.5, 12) };

static const vec2f_t goal_positions4[] = { vec2f_t(31-19-5+1, 21.5), vec2f_t(31-19-5+1, 12.5) };
static const vec2f_t goal_positions5[] = { vec2f_t(31-26-4+1, 6.5), vec2f_t(31-26-4+1, 13.5) };
static const vec2f_t goal_positions6[] = { vec2f_t(31-1.5-4+1, 12), vec2f_t(31-8.5-4+1, 12) };

static const moving_platform_params_t level1_platform_params[] = {
    moving_platform_params_t(vec2f_t(19, 19), vec2f_t(5, 1), 3, goal_positions1, 2),
    moving_platform_params_t(vec2f_t(26, 9), vec2f_t(4, 1), 3, goal_positions2, 2),
    moving_platform_params_t(vec2f_t(8, 12), vec2f_t(4, 1), 3, goal_positions3, 2)
};

static const moving_platform_params_t level2_platform_params[] = {
    moving_platform_params_t(vec2f_t(31-19-5+1, 19), vec2f_t(5, 1), 3, goal_positions4, 2),
    moving_platform_params_t(vec2f_t(31-26-4+1, 9), vec2f_t(4, 1), 3, goal_positions5, 2),
    moving_platform_params_t(vec2f_t(31-8-4+1, 12), vec2f_t(4, 1), 3, goal_positions6, 2)
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
            "#            ####              #",
            "#   #############              #",
            "# ^ #############            $ #",
            "################################",
        },
        3, level1_platform_params, 4
    },
    {
        {
            "################################", 
            "#>>                            #",
            "#>>                            #",
            "#>>    *    *                  #",
            "#>>    *  $ *                  #",
            "#####################          #",
            "#                    ##        #",
            "#                      ##      #",
            "#                        ##    #",
            "#                              #",
            "#        $                   ###",
            "#      ############            #",
            "#                 #            #",
            "#                 #            #",
            "#                 #************#",
            "#######                        #",
            "#######                        #",
            "#                              #",
            "#                              #",
            "#                $             #",
            "#              ####            #",
            "#              #############   #",
            "# $            ############# ^ #",
            "################################",
        },
        3, level2_platform_params, 4
    }
};
