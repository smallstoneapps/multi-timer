#include <pebble.h>
#include <pebble-assist.h>

#include "settings.h"

#include "timer.h"
#include "persist.h"

typedef struct {
  bool save_timers_auto; // Automatically save timers on exit and load them when the app restarts?
  bool resume_timers; // Automatically resume running timers when app starts?
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  bool timers_hours; // Use hours in timers?
} OldSettings;

static void migrate_settings(OldSettings old);

// Set the default settings for the app.
Settings _settings = {
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBE_SHORT,
  .timers_duration = 10 * 60,
  .timers_hours = false,
  .show_clock = false
};

void settings_load(void) {
  OldSettings old_settings;
  if (persist_exists(PERSIST_SETTINGS)) {
    if (! persist_exists(PERSIST_SETTINGS_VERSION)) {
      int res = persist_read_data(PERSIST_SETTINGS, &old_settings, sizeof(old_settings));
      if (res >= 0) {
        migrate_settings(old_settings);
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
  persist_write_int(PERSIST_SETTINGS_VERSION, 2);
}

static void migrate_settings(OldSettings old) {
  _settings.timers_hours = old.timers_hours;
  _settings.timers_vibration = old.timers_vibration;
  _settings.timers_start_auto = old.timers_start_auto;
}