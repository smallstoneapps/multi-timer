#ifndef VIBE_WINDOW_H
#define VIBE_WINDOW_H

#include "pebble_os.h"

#define COOKIE_VW_VIBRATE 8423 // VIBE

void init_vibe_window(AppContextRef ctx);
void show_vibe_window();
void vibe_window_timer();

#endif // VIBE_WINDOW_H