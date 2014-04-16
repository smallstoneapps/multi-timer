/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * timers.h
 ***/

#pragma once

#include "timer.h"

#define MAX_TIMERS 16

Timer* timers_get(int pos);
int timers_get_count();
void timers_clear();
void timers_add(Timer* timer);
void timers_remove(int pos);

void timers_restore(void);
void timers_save(void);
