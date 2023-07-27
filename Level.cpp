#include "Level.h"

void level_t::init()
{
    this->platforms = (moving_platform_t *) malloc(
            this->num_platforms() * sizeof(moving_platform_t)
            );
    for (u32 i = 0; i < this->num_platforms(); i++)
        this->platforms[i] = moving_platform_t(&this->params->platform_params[i]);
}

void level_t::cleanup()
{
    if (this->platforms) {
        free(this->platforms);
        this->platforms = nullptr;
    }
}

void level_t::reset()
{
    for (u32 i = 0; i < this->num_platforms(); i++)
        this->platforms[i].reset();
}
