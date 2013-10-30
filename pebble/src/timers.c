/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timers.c
 ***/

#include <pebble_os.h>
#include "globals.h"
#include "timers.h"

Timer* timers[MAX_TIMERS];
int num_timers = 0;

Timer* get_timer(int pos) {
  if (pos >= num_timers) {
    return NULL;
  }
  return timers[pos];
}

int get_timer_count() {
  return num_timers;
}

void clear_timers() {
  for (int t = 0; t < num_timers; t += 1) {
    if (timers[t]->app_timer != NULL) {
      app_timer_cancel(timers[t]->app_timer);
    }
    free(timers[t]);
  }
  num_timers = 0;
}

bool add_timer(int duration, TimerVibration vibrate) {
  if (duration <= 0) {
    return false;
  }
  if (num_timers >= MAX_TIMERS) {
    return false;
  }
  Timer* t = malloc(sizeof(Timer));
  t->length = duration;
  t->time_left = duration;
  t->status = TIMER_STOPPED;
  t->vibrate = vibrate;
  t->direction = TIMER_DOWN;
  t->app_timer = NULL;
  timers[num_timers] = t;
  num_timers += 1;
  return true;
}

bool add_stopwatch() {
  if (num_timers >= MAX_TIMERS) {
    return false;
  }
  Timer* t = malloc(sizeof(Timer));
  t->time_left = t->length = 0;
  t->status = TIMER_STOPPED;
  t->direction = TIMER_UP;
  t->app_timer = NULL;
  timers[num_timers] = t;
  num_timers += 1;
  return true;
}

void remove_timer(int pos) {
  if (pos >= num_timers) {
    return;
  }
  free(timers[pos]);
  for (int p = pos + 1; p < num_timers; p += 1) {
    timers[p - 1] = timers[p];
  }
  num_timers -= 1;
}

void timers_restore(void) {
  clear_timers();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "read: %d", persist_exists(STORAGE_TIMER_COUNT));
  if (! persist_exists(STORAGE_TIMER_COUNT)) {
    return;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "count: %d", persist_read_int(STORAGE_TIMER_COUNT));
  num_timers = persist_read_int(STORAGE_TIMER_COUNT);
  for (int t = 0; t < num_timers; t += 1) {
    Timer* timer = malloc(sizeof(Timer));
    persist_read_data(STORAGE_TIMER_START + t, sizeof(timer), timer);
    timers[t] = timer;
    timers[t]->app_timer = NULL;
    timer_reset(timers[t]);
  }
}

void timers_save(void) {
  persist_write_int(STORAGE_TIMER_COUNT, num_timers);
  for (int t = 0; t < num_timers; t += 1) {
    persist_write_data(STORAGE_TIMER_START + t, sizeof(timers[t]), timers[t]);
  }
}

void timers_add(Timer* timer) {
  timer->status = TIMER_STOPPED;
  timer->app_timer = NULL;
  if (timer->direction == TIMER_UP) {
    timer->length = 0;
    timer->time_left = 0;
  }
  else {
    timer->time_left = timer->length;
  }
  timers[num_timers] = timer;
  num_timers += 1;
}
