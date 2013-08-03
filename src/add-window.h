#ifndef ADD_WINDOW_H
#define ADD_WINDOW_H

#include "pebble_os.h"

#define COOKIE_AW_FLICKER 15

void init_add_window(Window* window, AppContextRef ctx);
void show_add_window(Window* window);
void add_window_timer();

#endif // ADD_WINDOW_H