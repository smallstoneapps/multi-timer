/*

Multi Timer v3.0

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

// TODO: Fix the is_blank thing because I'm an idiot.

#include <pebble.h>
#include "globals.h"
#include "timers.h"
#include "common.h"
#include "settings.h"
#include "generated/appinfo.h"
#include <pebble-assist.h>
#include <data-processor.h>
#include <linked-list.h>
#include <message-queue.h>

#define TIMER_BLOCK_SIZE 5

typedef struct TimerBlock {
  Timer timers[TIMER_BLOCK_SIZE];
  uint8_t count;
  int time;
} TimerBlock;

static LinkedRoot* timers = NULL;
static bool is_blank = true;

void timers_init(void) {
  timers = linked_list_create_root();
}

Timer* timers_get(uint8_t pos) {
  return (Timer*) linked_list_get(timers, pos);
}

int timers_get_count() {
  return linked_list_count(timers);
}

void timers_add(Timer* timer) {
  linked_list_append(timers, timer);
  is_blank = false;
}

void timers_clear() {
  while (timers_get_count() > 0) {
    timers_remove(0);
  }
}

void timers_remove(uint8_t pos) {
  Timer* tmr = timers_get(pos);
  free_safe(tmr);
  linked_list_remove(timers, pos);
}

Timer* timers_find(const uint16_t id) {
  uint8_t count = timers_get_count();
  for (uint8_t t = 0; t < count; t += 1) {
    if (id == timers_get(t)->id) {
      return timers_get(t);
    }
  }
  return NULL;
}

status_t timers_restore(void) {
  timers_clear();

  if (! persist_exists(STORAGE_TIMER_START)) {
    return 0;
  }

  is_blank = false;

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
    TimerBlock* timer_block = malloc(sizeof(TimerBlock));
    timer_block->count = 0;
    persist_write_data(STORAGE_TIMER_START, timer_block, sizeof(TimerBlock));
    free_safe(timer_block);
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
    free_safe(timerBlock);
    block += 1;
  }
  return 0;
}

void timers_send_list(void) {
  const uint8_t timer_count = timers_get_count();
  if (0 == timer_count) {
    mqueue_add("TMR", "LIST!", " ");
  }
  else {
    size_t timer_string_size = (TIMER_STR_LENGTH * timer_count) + timer_count;
    char* timer_string = malloc(sizeof(char) * timer_string_size);
    if (NULL == timer_string) {
      ERROR("Could not allocate enough memory for the timer string!");
      return;
    }
    timer_string[0] = 0;
    for (uint8_t t = 0; t < timer_count; t += 1) {
      strcat(timer_string, timer_serialize(timers_get(t), DELIMITER_INNER));
      if (t < timer_count - 1) {
        timer_string[strlen(timer_string)] = DELIMITER_OUTER;
      }
    }
    mqueue_add("TMR", "LIST!", timer_string);
    free_safe(timer_string);
  }
}

void timers_load_list(char* data) {
  timers_clear();
  ProcessingState* ps = data_processor_create(data, DELIMITER_OUTER);
  data_processor_get_int(ps);
  uint8_t num_timers = data_processor_get_int(ps);
  for (uint8_t t = 0; t < num_timers; t += 1) {
    Timer* timer = timer_deserialize(data_processor_get_string(ps), DELIMITER_INNER);
    timers_add(timer);
    timer_reset(timer);
  }
  data_processor_destroy(ps);
}

void timers_get_list(void) {
  mqueue_add("TMR", "LIST?", " ");
}

bool timers_are_blank(void) {
  return is_blank;
}