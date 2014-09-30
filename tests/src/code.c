#include "settings.h"
#include "timer.h"

Settings _settings = {
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBE_SHORT,
  .timers_duration = 10 * 60,
  .timers_hours = false,
  .show_clock = false
};


void win_vibrate_show(void) {

}

Settings* settings(void) {
  return &_settings;
}