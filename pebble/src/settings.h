/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * settings.h
 ***/

#pragma once

#include "timer.h"

typedef struct {
  bool save_timers_auto; // Automatically save timers on exit and load them when the app restarts?
  bool resume_timers; // Automatically resume running timers when app starts?
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  bool timers_hours; // Use hours in timers?
} Settings;

Settings* settings();
void settings_load(void);
void settings_save(void);
