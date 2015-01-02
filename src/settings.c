/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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
#include <pebble-assist.h>

#include "settings.h"

#include "timer.h"
#include "migration.h"
#include "persist.h"

static void migrate_settings_01(OldSettings old);
static void migrate_settings_02(SettingsTiny tiny);

// Set the default settings for the app.
Settings _settings = {
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBE_SHORT,
  .timers_duration = 10 * 60,
  .timers_hours = false,
  .show_clock = false
};

void settings_load(void) {
  if (persist_exists(PERSIST_SETTINGS)) {
    if (! persist_exists(PERSIST_SETTINGS_VERSION)) {
      DEBUG("Migrating settings from 2.X to 3.X");
      OldSettings old_settings;
      int res = persist_read_data(PERSIST_SETTINGS, &old_settings, sizeof(old_settings));
      if (res >= 0) {
        migrate_settings_01(old_settings);
        return;
      }
    }
    else if (persist_read_int(PERSIST_SETTINGS_VERSION) == SETTINGS_VERSION_TINY) {
      SettingsTiny settings_tiny;
      int res = persist_read_data(PERSIST_SETTINGS, &settings_tiny, sizeof(settings_tiny));
      if (res >= 0) {
        migrate_settings_02(settings_tiny);
        return;
      }
    }
    int res = persist_read_data(PERSIST_SETTINGS, &_settings, sizeof(_settings));
    if (res < 0) {
      LOG("Settings load failed: %d", res);
    }
  }
}

Settings* settings() {
  return &_settings;
}

void settings_save(void) {
  int res = persist_write_data(PERSIST_SETTINGS, &_settings, sizeof(_settings));
  if (res < 0) {
    LOG("Settings load failed: %d", res);
  }
  persist_write_int(PERSIST_SETTINGS_VERSION, SETTINGS_VERSION_CURRENT);
}

static void migrate_settings_01(OldSettings old) {
  _settings.timers_hours = old.timers_hours;
  _settings.timers_vibration = old.timers_vibration;
  _settings.timers_start_auto = old.timers_start_auto;
}

static void migrate_settings_02(SettingsTiny tiny) {
  _settings.timers_hours = tiny.timers_hours;
  _settings.timers_vibration = tiny.timers_vibration;
  _settings.timers_start_auto = tiny.timers_start_auto;
  _settings.timers_duration = tiny.timers_duration;
  _settings.show_clock = tiny.show_clock;
}
