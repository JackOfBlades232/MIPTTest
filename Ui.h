#pragma once
#include "Engine.h"
#include "Utils.h"

// To avoid needless pointer passing, the UI functions will be writing
// directly into the backbuffer

void ui_draw_score(s32 score);
void ui_draw_timer(u32 minutes, u32 seconds);
