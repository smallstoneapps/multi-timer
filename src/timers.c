/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * timers.c
 ***/

#include <pebble.h>
#include "globals.h"
#include "timers.h"
#include "settings.h"
#include "libs/pebble-assist/pebble-assist.h"

typedef struct TimerList TimerList;

struct TimerList {
  Timer* timer;
  TimerList* next;
};

static TimerList* timers = NULL;
static int num_timers = 0;

Timer* timers_get(int pos) {
  TimerList* tmp = timers;
  for (uint8_t t = 0; t < pos; t += 1) {
    if (NULL == tmp) {
      return NULL;
    }
    tmp = tmp->next;
  }
  return (NULL == tmp) ? NULL : tmp->timer;
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

  TimerList* tl = malloc(sizeof(TimerList));
  tl->timer = timer;
  tl->next = NULL;

  if (timers == NULL) {
    timers = tl;
  }
  else {
    TimerList* tail = timers;
    while (tail->next != NULL) {
      tail = tail->next;
    }
    tail->next = tl;
  }
  num_timers += 1;
}

void timers_clear() {
  while (timers != NULL) {
    free(timers->timer);
    TimerList* tmp = timers;
    timers = timers->next;
    free(tmp);
  }
  num_timers = 0;
}

void timers_remove(int pos) {

  TimerList* current = timers;
  TimerList* previous = NULL;

  for (uint8_t t = 0; t < pos; t += 1) {
    previous = current;
    current = current->next;
  }
  timer_destroy(current->timer);

  if (previous == NULL) {
    if (current->next == NULL) {
      timers = NULL;
    }
    else {
      timers = current->next;
    }
  }
  else {
    timers->next = current->next;
  }

  free(current);
  num_timers -= 1;

}

status_t timers_restore(void) {
  timers_clear();
  if (! persist_exists(STORAGE_TIMER_COUNT)) {
    return 0;
  }
  uint8_t timer_count = persist_read_int(STORAGE_TIMER_COUNT);
  int seconds_elapsed = 0;
  if (settings()->resume_timers) {
    int save_time = persist_read_int(STORAGE_SAVE_TIME);
    seconds_elapsed = time(NULL) - save_time;
  }
  for (int t = 0; t < timer_count; t += 1) {
    Timer* timer = malloc(sizeof(Timer));
    timers_add(timer);
    persist_read_data(STORAGE_TIMER_START + t, timer, sizeof(Timer));
    timer->app_timer = NULL;
    if (settings()->resume_timers) {
      if (timer->status == TIMER_STATUS_RUNNING) {
        if (timer->direction == TIMER_DIRECTION_UP) {
          timer->time_left += seconds_elapsed;
        }
        else {
          timer->time_left -= seconds_elapsed;
          if (timer->time_left <= 0) {
            timer->time_left = 0;
            timer->status = TIMER_STATUS_FINISHED;
          }
        }
        if (timer->status == TIMER_STATUS_RUNNING) {
          timer_resume(timer);
        }
      }
    }
    else {
      timer_reset(timer);
    }
  }
  return 0;
}

status_t timers_save(void) {
  status_t status = persist_write_int(STORAGE_TIMER_COUNT, num_timers);
  if (status < 0) {
    return status;
  }
  for (int t = 0; t < num_timers; t += 1) {
    persist_write_data(STORAGE_TIMER_START + t, timers_get(t), sizeof(Timer));
  }
  status = persist_write_int(STORAGE_SAVE_TIME, time(NULL));
  return status;
}

