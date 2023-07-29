#include "Level.h"

void level_t::init()
{
    this->platforms = (moving_platform_t *) malloc(
            this->num_platforms() * sizeof(moving_platform_t)
            );
    for (u32 i = 0; i < this->num_platforms(); i++)
        this->platforms[i] = moving_platform_t(&this->params->platform_params[i]);

    this->num_collectables = 0;
    for (u32 y = 0; y < GEOM_HEIGHT; y++)
        for (u32 x = 0; x < GEOM_WIDTH; x++) {
            if ((*this->geom_map())[y][x] == COLLECTABLE_GLYPH)
                this->num_collectables++;
        }

    this->collectables = (collectable_t *) malloc(
            this->num_collectables * sizeof(collectable_t)
            );
    u32 i = 0;
    for (u32 y = 0; y < GEOM_HEIGHT; y++)
        for (u32 x = 0; x < GEOM_WIDTH; x++) {
            if ((*this->geom_map())[y][x] == COLLECTABLE_GLYPH) {
                f32 pos_x = (f32) x + 0.5;
                f32 pos_y = (f32) y + 0.5;

                this->collectables[i++] = collectable_t(
                        vec2f_t(pos_x, pos_y), COLLECTABLE_RAD,
                        vec2f_t(0, -COLLECTABLE_INIT_SPEED), 
                        vec2f_t(0, COLLECTABLE_ACCELERATION)
                        );
            }
        }
}

void level_t::cleanup()
{
    if (this->platforms) {
        free(this->platforms);
        this->platforms = nullptr;
    }
    if (this->collectables) {
        free(this->collectables);
        this->collectables = nullptr;
    }
}

void level_t::reset()
{
    for (u32 i = 0; i < this->num_platforms(); i++)
        this->platforms[i].reset();
    for (u32 i = 0; i < this->num_collectables; i++)
        this->collectables[i].reset();
}
