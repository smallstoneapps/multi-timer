/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timers.c
 ***/

#include <pebble.h>
#include "globals.h"
#include "timers.h"
#include "settings.h"

static Timer* timers[MAX_TIMERS];
static int num_timers = 0;

Timer* timers_get(int pos) {
  if (pos >= num_timers) {
    return NULL;
  }
  return timers[pos];
}

int timers_get_count() {
  return num_timers;
}

void timers_add(Timer* timer) {
  timer->status = TIMER_STATUS_STOPPED;
  timer->app_timer = NULL;
  if (timer->direction == TIMER_DIRECTION_UP) {
    timer->length = 0;
    timer->time_left = 0;
  }
  else {
    timer->time_left = timer->length;
  }
  timers[num_timers] = timer;
  num_timers += 1;
}

void timers_clear() {
  for (int t = 0; t < num_timers; t += 1) {
    timer_reset(timers[t]);
    free(timers[t]);
  }
  num_timers = 0;
}

void timers_remove(int pos) {
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
  timers_clear();
  if (! persist_exists(STORAGE_TIMER_COUNT)) {
    return;
  }
  num_timers = persist_read_int(STORAGE_TIMER_COUNT);
  int seconds_elapsed = 0;
  if (settings()->resume_timers) {
    int save_time = persist_read_int(STORAGE_SAVE_TIME);
    seconds_elapsed = time(NULL) - save_time;
  }
  for (int t = 0; t < num_timers; t += 1) {
    /*int str_size = persist_get_size(STORAGE_TIMER_START + t);
    char* buffer = malloc(str_size);
    persist_read_string(STORAGE_TIMER_START + t, str_size, buffer);
    timers[t] = timer_unstringify(buffer);*/
    timers[t] = malloc(sizeof(Timer));
    persist_read_data(STORAGE_TIMER_START + t, sizeof(timers[t]), timers[t]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %s", t, timer_describe(timers[t]));
    timers[t]->app_timer = NULL;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", seconds_elapsed);
    /*if (settings()->resume_timers) {
      if (timers[t]->status == TIMER_STATUS_RUNNING) {
        if (timers[t]->direction == TIMER_DIRECTION_UP) {
          timers[t]->time_left += seconds_elapsed;
        }
        else {
          timers[t]->time_left -= seconds_elapsed;
          if (timers[t]->time_left <= 0) {
            timers[t]->time_left = 0;
            timers[t]->status = TIMER_STATUS_FINISHED;
          }
        }
        if (timers[t]->status == TIMER_STATUS_RUNNING) {
          timer_resume(timers[t]);
        }
      }
    }
    else {
      timer_reset(timers[t]);
    }*/
  }
}

void timers_save(void) {
  persist_write_int(STORAGE_TIMER_COUNT, num_timers);
  for (int t = 0; t < num_timers; t += 1) {
    //persist_write_string(STORAGE_TIMER_START + t, timer_stringify(timers[t]));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %s", t, timer_describe(timers[t]));
    persist_write_data(STORAGE_TIMER_START + t, sizeof(timers[t]), timers[t]);
  }
  persist_write_int(STORAGE_SAVE_TIME, time(NULL));
}

