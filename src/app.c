/*

Multi Timer v3.0

http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)

Copyright © 2013 - 2014 Matthew Tole

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

--------------------

src/app.c

*/

#include <pebble.h>
#include "generated/appinfo.h"
#include "globals.h"
#include <bitmap-loader.h>
#include <pebble-assist.h>
#include <message-queue.h>
#include <data-processor.h>
#include "windows/win-main.h"
#include "windows/win-error.h"
#include "windows/win-vibration.h"
#include "settings.h"
#include "timers.h"
#include "common.h"
#include "analytics.h"

static void handle_init(void);
static void handle_deinit(void);
static void message_handler_timers(char* operation, char* data);
static void message_handler_ops(char* operation, char* data);

//! Program main entry point.
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

//! App initialisation function.
//! Handles the loading of settings, timers and the primary window.
void handle_init() {

  srand(time(NULL));

  // Init all the things!
  timers_init();
  mqueue_init();
  bitmaps_init();
  settings_load();
  win_main_init();
  win_vibration_init();

  // Register some message queue handler functions.
  // mqueue_register_handler("OPS", message_handler_ops);
  mqueue_register_handler("TMR", message_handler_timers);

  // If the settings tell us to load the timers, load the damn timers.
  if (settings()->save_timers_auto) {
    timers_restore();
  }

  // Show the main window.
  win_main_show();

  // If the persistence test fails, show the error window and point people
  // to the page telling them how to fix the issue.
  StatusCode test_status = test_persist();
  if (S_SUCCESS > test_status) {
    win_error_init();
    win_error_set_text("Persistent storage is no longer working correctly on \
your watch, so timers and settings will not be saved by this app.\n\
Please factory reset your watch and reload apps from the locker to \
correct the issue. For more information visit:\nbit.ly/pblstorage");
    analytics_track_event("error", "persistence_failed");
    win_error_show();
  }
}

//! App deinitialise functions
//! Handles saving the timers and settings.
//! No longer bothering to destroy things because it's stupid.
void handle_deinit() {
  if (settings()->save_timers_auto) {
    timers_save();
  }
  settings_save();
}

static void message_handler_timers(char* operation, char* data) {
  if (0 == strcmp(operation, "LIST?")) {
    if (timers_are_blank()) {
      timers_get_list();
    }
    else {
      timers_send_list();
    }
  }
  else if (0 == strcmp(operation, "LIST!")) {
    timers_load_list(data);
    win_main_update();
  }
  else if (0 == strcmp(operation, "LABEL")) {
    ProcessingState* ps = data_processor_create(data, DELIMITER_OUTER);
    uint16_t id = data_processor_get_int(ps);
    char* label = data_processor_get_string(ps);
    Timer* timer = timers_find(id);
    if (NULL != timer) {
      strcpy(timer->label, label);
    }
    free_safe(label);
    win_main_update();
    data_processor_destroy(ps);
  }
  else {
    DEBUG("TMR :: %s :: %s", operation, data);
  }
  DEBUG("TMR :: %s :: %s", operation, data);
}

static void message_handler_ops(char* operation, char* data) {
  DEBUG("OPS :: %s :: %s", operation, data);
}
