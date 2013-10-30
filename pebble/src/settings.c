/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * settings.c
 ***/

#include <pebble_os.h>
#include "globals.h"
#include "settings.h"

Settings _settings = {
  .save_timers_auto = true,
  .timers_start_auto = false
};

void settings_load(void) {
  if (persist_exists(STORAGE_SETTINGS)) {
    persist_read_data(STORAGE_SETTINGS, sizeof(_settings), &_settings);
  }
}

Settings* settings() {
  return &_settings;
}

void settings_save(void) {
  persist_write_data(STORAGE_SETTINGS, sizeof(_settings), &_settings);
}
