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

void ui_draw_score(s32 score)
{
    rect_t r(score_rect_template);

    for (u32 i = 0; i < SCORE_PLACK_DIGITS; i++) {
        draw_char(&r, (score % 10) + '0', FONT_COLOR);
        score /= 10;
        r.pos.x -= UI_CHAR_SIZE_X;
    }

    ASSERTF(score == 0, "Score too large to display in chosen rect, truncate it manually\n");
}

void ui_draw_timer(u32 seconds)
{
    ASSERTF(seconds <= TIMER_MAX_SECONDS, "Trying to display timer with %d seconds, max: %d\n", seconds, TIMER_MAX_SECONDS);

    rect_t r(timer_rect_template);

    draw_char(&r, (seconds % 10) + '0', FONT_COLOR);
    seconds /= 10;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 6) + '0', FONT_COLOR);
    seconds /= 6;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, ':', FONT_COLOR);
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 10) + '0', FONT_COLOR);
    seconds /= 10;
    r.pos.x -= UI_CHAR_SIZE_X;

    draw_char(&r, (seconds % 10) + '0', FONT_COLOR);
    seconds /= 10;

    ASSERT(seconds == 0);
}

void ui_draw_win_screen()
{
    draw_rect(&win_screen_bg_rect, WIN_UI_BG_COLOR);

    rect_t r(win_screen_line1_rect_template);
    for (u32 i = 0; i < WIN_SCREEN_LINE1_LEN; i++) {
        draw_char(&r, win_screen_line1[i], FONT_COLOR);
        r.pos.x += WIN_UI_CHAR_SIZE_X;
    }

    r = rect_t(win_screen_line2_rect_template);
    for (u32 i = 0; i < WIN_SCREEN_LINE2_LEN; i++) {
        draw_char(&r, win_screen_line2[i], FONT_COLOR);
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
