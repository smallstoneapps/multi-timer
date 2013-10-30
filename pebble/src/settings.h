/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 ***/

#pragma once

typedef struct {
  bool save_timers_auto;
  bool timers_start_auto;
} Settings;

void settings_load(void);
Settings* settings();
void settings_save(void);
