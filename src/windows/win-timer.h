/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * windows/win-timer.h
 ***/

#pragma once

#include <pebble.h>
#include "../timer.h"

void win_timer_init(void);
void win_timer_set_timer(Timer* tmr, uint8_t pos);
void win_timer_show(void);
void win_timer_destroy(void);