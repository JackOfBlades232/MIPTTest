#include "Draw.h"
#include "Engine.h"
#include "Params.h"

void draw_rect(const rect_t *r, u32 color)
{
    u32 screen_pos_x = r->pos.x*SECTOR_PIX_SIZE;
    u32 screen_pos_y = r->pos.y*SECTOR_PIX_SIZE;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    u32 screen_size_x = r->size.x*SECTOR_PIX_SIZE;
    u32 screen_size_y = r->size.y*SECTOR_PIX_SIZE;

    if (screen_pos_x < 0) {
        screen_size_x += screen_pos_x;
        screen_pos_x = 0;
    } else if (screen_pos_x > SCREEN_WIDTH-screen_size_x)
        screen_size_x = SCREEN_WIDTH-screen_pos_x;
    if (screen_pos_y < 0) {
        screen_size_y += screen_pos_y;
        screen_pos_y = 0;
    } else if (screen_pos_y > SCREEN_HEIGHT-screen_size_y)
        screen_size_y = SCREEN_HEIGHT-screen_pos_y;

    ASSERT(screen_pos_x >= 0 && screen_pos_x+screen_size_x <= SCREEN_WIDTH);
    ASSERT(screen_pos_y >= 0 && screen_pos_y+screen_size_y <= SCREEN_HEIGHT); 

    for (u32 y = screen_pos_y; y < screen_pos_y+screen_size_y; y++) {
        u32 *pixel = buffer[y] + screen_pos_x;
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++)
            *(pixel++) = color;
    }
}

void draw_circle(const circle_t *c, u32 color)
{
    vec2f_t screen_center = c->center*SECTOR_PIX_SIZE;
    f32 screen_rad = c->rad*SECTOR_PIX_SIZE;

    f32 r2 = screen_rad * screen_rad;
    f32 rel_y = screen_rad-EPSILON;
    f32 rel_y2 = rel_y*rel_y;

    u32 min_y = MAX(screen_center.y - screen_rad, 0);
    u32 max_y = MIN(screen_center.y + screen_rad, SCREEN_HEIGHT-1);

    for (u32 y = min_y; y <= max_y; y++) {
        f32 off_x = floor(sqrt(r2 - rel_y2));

        u32 min_x = MAX(screen_center.x - off_x, 0);
        u32 max_x = MIN(screen_center.x + off_x, SCREEN_WIDTH-1);
        u32 *pixel = buffer[y] + min_x;

        for (u32 x = min_x; x <= max_x; x++)
            *(pixel++) = color;

        rel_y2 -= 2*rel_y - 1;
        rel_y--;
    }
}
