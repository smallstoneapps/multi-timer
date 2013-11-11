/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * settings.c
 ***/

#include <pebble.h>
#include "globals.h"
#include "settings.h"
#include "timer.h"

Settings _settings = {
  .save_timers_auto = true,
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBE_OFF,
  .timers_hours = false
};

void settings_load(void) {
  if (persist_exists(STORAGE_SETTINGS)) {
    int res = persist_read_data(STORAGE_SETTINGS, sizeof(_settings), &_settings);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "settings_load: %d", res);
  }
  else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "settings_load: No settings.");
  }
}

Settings* settings() {
  return &_settings;
}

void settings_save(void) {
  int res = persist_write_data(STORAGE_SETTINGS, sizeof(_settings), &_settings);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "settings_save: %d", res);
}
