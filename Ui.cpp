#include "Ui.h"
#include "FontAtlas.h"
#include "Params.h"
#include "Draw.h"
#include "Geom.h"
#include "Utils.h"

static const rect_t score_rect_template( 
        (SCORE_PLACK_X+SCORE_PLACK_DIGITS-1) * UI_CHAR_SIZE_X, SCORE_PLACK_Y * UI_CHAR_SIZE_Y,
        UI_CHAR_SIZE_X, UI_CHAR_SIZE_Y
        );
static const rect_t timer_rect_template( 
        (TIMER_X+TIMER_FMT_LEN-1) * UI_CHAR_SIZE_X, TIMER_Y * UI_CHAR_SIZE_Y,
        UI_CHAR_SIZE_X, UI_CHAR_SIZE_Y
        );

static const rect_t win_screen_bg_rect(
        WIN_SCREEN_BG_X, WIN_SCREEN_BG_Y, WIN_SCREEN_BG_SIZE_X, WIN_SCREEN_BG_SIZE_Y
        );
static const rect_t win_screen_line1_rect_template(
        WIN_SCREEN_LINE1_X, WIN_SCREEN_BG_Y + WIN_SCREEN_LINE1_OFF,
        WIN_UI_CHAR_SIZE_X, WIN_UI_CHAR_SIZE_Y
        );
static const rect_t win_screen_line2_rect_template(
        WIN_SCREEN_LINE2_X, WIN_SCREEN_BG_Y + WIN_SCREEN_LINE2_OFF,
        WIN_UI_CHAR_SIZE_X, WIN_UI_CHAR_SIZE_Y
        );

static void draw_char(const rect_t *r, char c);

void ui_draw_score(s32 score)
{
    rect_t r(score_rect_template);

    for (u32 i = 0; i < SCORE_PLACK_DIGITS; i++) {
        draw_char(&r, (score % 10) + '0');
        score /= 10;
        r.pos.x -= UI_CHAR_SIZE_X;
    }

    ASSERTF(score == 0, "Score too large to display in chosen rect, truncate it manually\n");
}

void ui_draw_timer(u32 seconds)
{
    ASSERTF(seconds <= TIMER_MAX_SECONDS, "Trying to display timer with %d seconds, max: %d\n", seconds, TIMER_MAX_SECONDS);

    rect_t r(timer_rect_template);

    draw_char(&r, (seconds % 10) + '0');
    seconds /= 10;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 6) + '0');
    seconds /= 6;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, ':');
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 10) + '0');
    seconds /= 10;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 10) + '0');
    seconds /= 10;

    ASSERT(seconds == 0);
}

void ui_draw_win_screen()
{
    draw_rect(&win_screen_bg_rect, WIN_UI_BG_COLOR);

    rect_t r(win_screen_line1_rect_template);
    for (u32 i = 0; i < WIN_SCREEN_LINE1_LEN; i++) {
        draw_char(&r, win_screen_line1[i]);
        r.pos.x += WIN_UI_CHAR_SIZE_X;
    }

    r = rect_t(win_screen_line2_rect_template);
    for (u32 i = 0; i < WIN_SCREEN_LINE2_LEN; i++) {
        draw_char(&r, win_screen_line2[i]);
        r.pos.x += WIN_UI_CHAR_SIZE_X;
    }
}

u32 ui_get_max_displayabe_score()
{
    u32 score = 1;
    for (u32 i = 0; i < SCORE_PLACK_DIGITS; i++)
        score *= 10;
    return score*10 - 1; // 999...99
}

static void draw_char(const rect_t *r, char c)
{
    ASSERT(c >= MIN_CHAR && c <= MAX_CHAR);

    u32 glyph_idx = c - MIN_CHAR;
    u32 glyph_idx_u = glyph_idx % GLYPHS_PER_ROW;
    u32 glyph_idx_v = glyph_idx / GLYPHS_PER_ROW;

    u32 glyph_off_u = FONT_ATLAS_GLYPH_WIDTH * glyph_idx_u;
    u32 glyph_off_v = FONT_ATLAS_GLYPH_HEIGHT * glyph_idx_v;


    // @TODO: factor out with draw rect
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
        for (u32 x = screen_pos_x; x < screen_pos_x+screen_size_x; x++) {
            u32 glyph_local_u = floor(((f32) x - screen_pos_x + 0.5)*FONT_ATLAS_GLYPH_WIDTH / screen_size_x);
            u32 glyph_local_v = floor(((f32) y - screen_pos_y + 0.5)*FONT_ATLAS_GLYPH_HEIGHT / screen_size_y);

            if (font_atlas[glyph_off_v+glyph_local_v][glyph_off_u+glyph_local_u] != ' ')
                *(pixel++) = FONT_COLOR;
            else
                pixel++;
        }
    }
}
