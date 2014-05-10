/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * app.c
 ***/

#include <pebble.h>
#include "generated/appinfo.h"
#include "globals.h"
#include "libs/bitmap-loader/bitmap-loader.h"
#include "libs/pebble-assist/pebble-assist.h"
#include "windows/win-timers.h"
#include "windows/win-error.h"
#include "settings.h"
#include "timers.h"

static void handle_init(void);
static void handle_deinit(void);
static StatusCode test_persist(void);

//! Program main entry point.
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

//! App initialisation function.
//! Handles the loading of settings, timers and the primary window.
void handle_init() {
  win_error_init();
  bitmaps_init();
  settings_load();
  if (settings()->save_timers_auto) {
    timers_restore();
  }
  win_timers_init();
  win_timers_show();
  StatusCode test_status = test_persist();
  if (S_SUCCESS > test_status) {
    win_error_set_text("Persistent storage is no longer working correctly on \
your watch, so timers and settings will not be saved by this app.\n\
Please factory reset your watch and reload apps from the locker to \
correct the issue. For more information visit:\nbit.ly/pblstorage");
    win_error_show();
  }
}

//! App deinitialise functions
//! Handles saving the timers and settings, and cleaning up the primary window.
void handle_deinit() {
  if (settings()->save_timers_auto) {
    timers_save();
  }
  settings_save();
  win_timers_destroy();
  bitmaps_cleanup();
}

//! Test whether or not the persistent storage is working, by writing
//! and reading a value.
//! @return status code of write, negative values are bad.
static StatusCode test_persist(void) {
  StatusCode write_status = persist_write_bool(STORAGE_TEST, true);
  bool test_bool = persist_read_bool(STORAGE_TEST);
  if (true != test_bool) {
    return -100;
  }
  return write_status;
}
