#pragma once

#include "timer.h"

#define SETTINGS_VERSION_CURRENT 3
#define SETTINGS_VERSION_TINY 2

typedef struct {
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  uint32_t timers_duration; // Default timer duration
  bool timers_hours; // Use hours in timers?
  bool show_clock; // Display a clock row in the main screen?
} Settings;

Settings* settings();
void settings_load(void);
void settings_save(void);
