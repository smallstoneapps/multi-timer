/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 ***/

#pragma once

#include "timer.h"

typedef struct {
            bool save_timers_auto;
            bool timers_start_auto;
  TimerVibration timers_vibration;
            bool timers_hours;
} Settings;

void settings_load(void);
Settings* settings();
void settings_save(void);
