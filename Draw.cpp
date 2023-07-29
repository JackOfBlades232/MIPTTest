#include "Draw.h"
#include "Engine.h"
#include "FontAtlas.h"
#include "Params.h"
#include "Utils.h"

static inline void calculate_rect_screen_dimensions(
        const rect_t *r, 
        s32 *screen_pos_x_out, s32 *screen_pos_y_out,
        s32 *screen_size_x_out, s32 *screen_size_y_out
        );

void draw_rect(const rect_t *r, u32 color)
{
    s32 screen_pos_x, screen_pos_y, screen_size_x, screen_size_y;
    calculate_rect_screen_dimensions(r, &screen_pos_x, &screen_pos_y, &screen_size_x, &screen_size_y);

    for (u32 y = screen_pos_y; y < screen_pos_y+screen_size_y; y++) {
        u32 *pixel = buffer[y] + screen_pos_x;
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++)
            *(pixel++) = color;
    }
}

void draw_circle(const circle_t *c, u32 color)
{
    vec2f_t screen_center = c->center*SECTOR_PIX_SIZE + vec2f_t(SCREEN_LEFT_PADDING, SCREEN_TOP_PADDING);
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
        rel_y -= 1;

        if (rel_y < -screen_rad+EPSILON)
            break;
    }
}

void draw_char(const rect_t *r, char c, u32 color)
{
    ASSERT(c >= MIN_CHAR && c <= MAX_CHAR);

    u32 glyph_idx = c - MIN_CHAR;
    u32 glyph_idx_u = glyph_idx % GLYPHS_PER_ROW;
    u32 glyph_idx_v = glyph_idx / GLYPHS_PER_ROW;

    u32 glyph_off_u = FONT_ATLAS_GLYPH_WIDTH * glyph_idx_u;
    u32 glyph_off_v = FONT_ATLAS_GLYPH_HEIGHT * glyph_idx_v;

    s32 screen_pos_x, screen_pos_y, screen_size_x, screen_size_y;
    calculate_rect_screen_dimensions(r, &screen_pos_x, &screen_pos_y, &screen_size_x, &screen_size_y);

    for (u32 y = screen_pos_y; y < screen_pos_y+screen_size_y; y++) {
        u32 *pixel = buffer[y] + screen_pos_x;
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++) {
            u32 glyph_local_u = floor(((f32) x - screen_pos_x + 0.5)*FONT_ATLAS_GLYPH_WIDTH / screen_size_x);
            u32 glyph_local_v = floor(((f32) y - screen_pos_y + 0.5)*FONT_ATLAS_GLYPH_HEIGHT / screen_size_y);

            // Possible improvement -- map ascii art to shades of color
            if (font_atlas[glyph_off_v+glyph_local_v][glyph_off_u+glyph_local_u] != ' ')
                *(pixel++) = color;
            else
                pixel++;
        }
    }
}

static inline void calculate_rect_screen_dimensions(
        const rect_t *r, 
        s32 *screen_pos_x_out, s32 *screen_pos_y_out,
        s32 *screen_size_x_out, s32 *screen_size_y_out
        )
{
    s32 screen_pos_x = r->pos.x*SECTOR_PIX_SIZE + SCREEN_LEFT_PADDING;
    s32 screen_pos_y = r->pos.y*SECTOR_PIX_SIZE + SCREEN_TOP_PADDING;
    if (screen_pos_x > SCREEN_WIDTH-1 || screen_pos_y > SCREEN_HEIGHT-1)
        return;

    s32 screen_size_x = r->size.x*SECTOR_PIX_SIZE;
    s32 screen_size_y = r->size.y*SECTOR_PIX_SIZE;

    if (screen_pos_x > SCREEN_WIDTH-screen_size_x)
        screen_size_x = SCREEN_WIDTH-screen_pos_x;
    if (screen_pos_y > SCREEN_HEIGHT-screen_size_y)
        screen_size_y = SCREEN_HEIGHT-screen_pos_y;

    if (screen_pos_x < 0) {
        screen_size_x += screen_pos_x;
        screen_pos_x = 0;
    }
    if (screen_pos_y < 0) {
        screen_size_y += screen_pos_y;
        screen_pos_y = 0;
    }

    ASSERT(screen_pos_x >= 0 && screen_pos_x+screen_size_x <= SCREEN_WIDTH);
    ASSERT(screen_pos_y >= 0 && screen_pos_y+screen_size_y <= SCREEN_HEIGHT); 

    *screen_pos_x_out = screen_pos_x;
    *screen_pos_y_out = screen_pos_y;
    *screen_size_x_out = screen_size_x;
    *screen_size_y_out = screen_size_y;
}
