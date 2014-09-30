#pragma once

#include <pebble.h>
#include "timer.h"

void menu_draw_row_icon_text(GContext* ctx, char* text, GBitmap* icon);
void timer_draw_row(Timer* timer, GContext* ctx);
void menu_draw_option(GContext* ctx, char* option, char* value);
void uppercase(char* str);