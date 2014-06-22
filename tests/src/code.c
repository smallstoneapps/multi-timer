#include "../../src/settings.h"

Settings _settings = {
  .save_timers_auto = true,
  .resume_timers = false,
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBRATION_SHORT,
  .timers_hours = false
};

Settings* settings(void) {
  return &_settings;
}

void win_vibrate_show(void) {
  }

bool win_vibrate_is_visible(void) {
  return false;
}

void win_timers_update(void) {
  
}