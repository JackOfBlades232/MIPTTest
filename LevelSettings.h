#pragma once
#include "Level.h"

static const vec2f_t goal_positions11[] = { vec2f_t(19, 21.5), vec2f_t(19, 12.5) };
static const vec2f_t goal_positions12[] = { vec2f_t(26, 6.5), vec2f_t(26, 13.5) };
static const vec2f_t goal_positions13[] = { vec2f_t(1.5, 12), vec2f_t(8.5, 12) };

static const moving_platform_params_t level1_platform_params[] = {
    moving_platform_params_t(vec2f_t(19, 19), vec2f_t(5, 1), 3, goal_positions11, 2),
    moving_platform_params_t(vec2f_t(26, 9), vec2f_t(4, 1), 3, goal_positions12, 2),
    moving_platform_params_t(vec2f_t(8, 12), vec2f_t(4, 1), 3, goal_positions13, 2)
};

static const vec2f_t goal_positions21[] = { vec2f_t(2, 14), vec2f_t(2, 17.5) };
static const vec2f_t goal_positions22[] = { vec2f_t(2.5, 21), vec2f_t(9.5, 21) };
static const vec2f_t goal_positions23[] = { vec2f_t(12, 11), vec2f_t(16, 11) };
static const vec2f_t goal_positions24[] = { vec2f_t(12, 6), vec2f_t(16, 6) };

static const moving_platform_params_t level2_platform_params[] = {
    moving_platform_params_t(vec2f_t(2, 16), vec2f_t(3, 1), 3, goal_positions21, 2),
    moving_platform_params_t(vec2f_t(2.5, 21), vec2f_t(4, 1), 3, goal_positions22, 2),
    moving_platform_params_t(vec2f_t(16, 11), vec2f_t(2, 1), 4, goal_positions23, 2),
    moving_platform_params_t(vec2f_t(12, 6), vec2f_t(2, 1), 4, goal_positions24, 2)
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
            "#    >>      ####              #",
            "#   #############              #",
            "# ^ #############            $ #",
            "################################",
        },
        3, level1_platform_params, 4
    },
    {
        {
            "################################", 
            "#         *                    #",
            "#         *                    #",
            "#         *                    #",
            "# ^       *          $         #",
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
            "#******************************#",
        },
        4, level2_platform_params, 4
    }
};
