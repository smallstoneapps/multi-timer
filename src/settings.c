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
