#ifndef TIMERS_H
#define TIMERS_H

#include "timer.h"

#define MAX_TIMERS 3

Timer* get_timer(int pos);
int get_timer_count();
void clear_timers();
bool add_timer(int duration, bool vibrate);
void remove_timer(int pos);

#endif