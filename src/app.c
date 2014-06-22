/*

Multi Timer v2.7.0
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
#include "libs/bitmap-loader/bitmap-loader.h"
#include "libs/pebble-assist/pebble-assist.h"
// #include "libs/message-queue/message-queue.h"
#include "windows/win-timers.h"
#include "windows/win-error.h"
#include "settings.h"
#include "timers.h"
#include "common.h"

#define OUTER_SEP "~"
#define INNER_SEP '`'

static void handle_init(void);
static void handle_deinit(void);
// static void message_handler_timers(char* operation, char* data);
// static void message_handler_ops(char* operation, char* data);

//! Program main entry point.
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

//! App initialisation function.
//! Handles the loading of settings, timers and the primary window.
void handle_init() {

  // If the persistence test fails, show the error window and point people
  // to the page telling them how to fix the issue.
  StatusCode test_status = test_persist();
  if (S_SUCCESS > test_status) {
    win_error_init();
    win_error_set_text("Persistent storage is no longer working correctly on \
your watch, so timers and settings will not be saved by this app.\n\
Please factory reset your watch and reload apps from the locker to \
correct the issue. For more information visit:\nbit.ly/pblstorage");
    win_error_show();
  }

  // Init all the things!
  timers_init();
  // mqueue_init();
  bitmaps_init();
  settings_load();
  win_timers_init();

  // Register some message queue handler functions.
  // mqueue_register_handler("OPS", message_handler_ops);
  // mqueue_register_handler("TMR", message_handler_timers);

  // If the settings tell us to load the timers, load the damn timers.
  if (settings()->save_timers_auto) {
    timers_restore();
  }

  // Show the main window.
  win_timers_show();
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
  DEBUG("TMR :: %s :: %s", operation, data);
}

static void message_handler_ops(char* operation, char* data) {
  DEBUG("OPS :: %s :: %s", operation, data);
}