/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * app.c
 ***/

#include <pebble.h>

#include "libs/bitmap-loader/bitmap-loader.h"
#include "windows/win-timers.h"
#include "settings.h"
#include "timers.h"

static void handle_init(void);
static void handle_deinit(void);

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

void handle_init() {
  bitmaps_init();
  settings_load();
  if (settings()->save_timers_auto) {
    timers_restore();
  }
  win_timers_init();
  win_timers_show();
}

void handle_deinit() {
  bitmaps_cleanup();
  win_timers_destroy();
  if (settings()->save_timers_auto) {
    timers_save();
  }
  settings_save();
}
