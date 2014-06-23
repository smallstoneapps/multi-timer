/*

Multi Timer v2.7.1
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

src/settings.c

*/

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
