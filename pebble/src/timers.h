/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timers.h
 ***/

#pragma once

#include "timer.h"

#define MAX_TIMERS 8

Timer* get_timer(int pos);
int get_timer_count();
void clear_timers();
bool add_timer(int duration, TimerVibration vibrate);
bool add_stopwatch();
void remove_timer(int pos);

void timers_restore(void);
void timers_save(void);
void timers_add(Timer* timer);