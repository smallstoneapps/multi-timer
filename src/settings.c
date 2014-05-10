/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * settings.c
 ***/

#include <pebble.h>
#include "libs/pebble-assist/pebble-assist.h"
#include "globals.h"
#include "settings.h"
#include "timer.h"

// Set the default settings for the app.
Settings _settings = {
  .save_timers_auto = true,
  .resume_timers = true,
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBRATION_SHORT,
  .timers_hours = false
};

void settings_load(void) {
  if (persist_exists(STORAGE_SETTINGS)) {
    int res = persist_read_data(STORAGE_SETTINGS, &_settings, sizeof(_settings));
    if (res < 0) {
      LOG("Settings load failed: %d", res);
    }
  }
}

Settings* settings() {
  return &_settings;
}

void settings_save(void) {
  int res = persist_write_data(STORAGE_SETTINGS, &_settings, sizeof(_settings));
  if (res < 0) {
    LOG("Settings load failed: %d", res);
  }
}
