/*

Multi Timer v2.7.0
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)

Copyright © 2013 - 2014 Matthew Tole

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

--------------------

src/timers.c

*/

#include <pebble.h>
#include "globals.h"
#include "timers.h"
#include "settings.h"
#include "libs/pebble-assist/pebble-assist.h"
#include "libs/linked-list/linked-list.h"

#define TIMER_BLOCK_SIZE 5

typedef struct TimerBlock {
  Timer timers[TIMER_BLOCK_SIZE];
  uint8_t count;
  int time;
} TimerBlock;

static LinkedRoot* timers = NULL;

void timers_init(void) {
  timers = linked_list_create_root();
}

Timer* timers_get(int pos) {
  return (Timer*) linked_list_get(timers, pos);
}

int timers_get_count() {
  return linked_list_count(timers);
}

void timers_add(Timer* timer) {
  linked_list_append(timers, timer);
}

void timers_clear() {
  while (timers_get_count() > 0) {
    timers_remove(0);
  }
}

void timers_remove(int pos) {
  Timer* tmr = timers_get(pos);
  free_safe(tmr);
  linked_list_remove(timers, pos);
}

status_t timers_restore(void) {
  timers_clear();

  if (! persist_exists(STORAGE_TIMER_START)) {
    return 0;
  }

  int block = 0;
  TimerBlock* timerBlock = malloc(sizeof(TimerBlock));
  persist_read_data(STORAGE_TIMER_START, timerBlock, sizeof(TimerBlock));

  uint8_t timer_count = timerBlock->count;
  int seconds_elapsed = 0;
  if (settings()->resume_timers) {
    int save_time = timerBlock->time;
    seconds_elapsed = time(NULL) - save_time;
  }

  for (int t = 0; t < timer_count; t += 1) {

    if (t > 0 && t % TIMER_BLOCK_SIZE == 0) {
      block += 1;
      free_safe(timerBlock);
      timerBlock = malloc(sizeof(TimerBlock));
      persist_read_data(STORAGE_TIMER_START + block, timerBlock, sizeof(TimerBlock));
    }

    Timer* timer = timer_clone(&timerBlock->timers[t % TIMER_BLOCK_SIZE]);
    timers_add(timer);

    timer->app_timer = NULL;
    if (! settings()->resume_timers) {
      timer_reset(timer);
      continue;
    }
    if (TIMER_STATUS_RUNNING != timer->status) {
      continue;
    }
    if (TIMER_DIRECTION_UP == timer->direction) {
      timer->time_left += seconds_elapsed;
    }
    else {
      if (true == timer->repeat) {
        timer->time_left -= (seconds_elapsed % timer->length);
        if (timer->time_left <= 0) {
          timer->time_left += timer->length;
        }
      }
      else {
        timer->time_left -= seconds_elapsed;
        if (0 >= timer->time_left) {
          timer->time_left = 0;
          timer->status = TIMER_STATUS_FINISHED;
          continue;
        }
      }
    }
    timer_resume(timer);
  }
  free_safe(timerBlock);
  return 0;
}

status_t timers_save(void) {
  int block = 0;
  uint8_t num_timers = timers_get_count();
  if (num_timers == 0) {
    persist_delete(STORAGE_TIMER_START);
    return 0;
  }
  for (int t = 0; t < num_timers; t += TIMER_BLOCK_SIZE) {
    TimerBlock* timerBlock = malloc(sizeof(TimerBlock));
    timerBlock->count = num_timers;
    timerBlock->time = time(NULL);
    for (int u = 0; u < TIMER_BLOCK_SIZE; u += 1) {
      if (t + u >= num_timers) {
        break;
      }
      timerBlock->timers[u] = *timers_get(t + u);
    }
    persist_write_data(STORAGE_TIMER_START + block, timerBlock, sizeof(TimerBlock));
    free(timerBlock);
    block += 1;
  }
  return 0;
}

