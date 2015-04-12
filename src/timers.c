/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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

#include <linked-list.h>

#include "timers.h"

#include "timer.h"
#include "persist.h"
#include "migration.h"

typedef struct {
  Timer timers[TIMER_BLOCK_SIZE];
  uint8_t total_timers;
  time_t save_time;
} TimerBlock;

static void timers_cleanup(void);
static void timers_migrate_1(void);
static void timers_migrate_2(void);
static void timers_migrate_3(void);
static TimerTimestamp new_timestamp(void);
static void timers_fire_update_handlers(void);

LinkedRoot* timers = NULL;
LinkedRoot* update_handlers = NULL;
LinkedRoot* highlight_handlers = NULL;

static TimerTimestamp current_time;
static bool update_marked;
static bool queue_updates;

void timers_init(void) {
  timers_cleanup();
  timers = linked_list_create_root();
  update_handlers = linked_list_create_root();
  highlight_handlers = linked_list_create_root();
  current_time = new_timestamp();
  queue_updates = false;
  update_marked = false;
}

uint8_t timers_count(void) {
  return linked_list_count(timers);
}

Timer* timers_get(uint8_t index) {
  if (! timers) {
    return NULL;
  }
  return linked_list_get(timers, index);
}

Timer* timers_find(uint16_t id) {
  uint8_t count = timers_count();
  for (uint8_t c = 0; c < count; c += 1) {
    Timer* timer = timers_get(c);
    if (timer->id == id) {
      return timer;
    }
  }
  return NULL;
}

int16_t timers_index_of(uint16_t id) {
  uint8_t count = timers_count();
  for (uint8_t c = 0; c < count; c += 1) {
    Timer* timer = timers_get(c);
    if (timer->id == id) {
      return c;
    }
  }
  return -1;
}

bool timers_add(Timer* timer) {
  linked_list_append(timers, timer);
  return true;
}

bool timers_remove(uint8_t position) {
  Timer* timer = timers_get(position);
  if (NULL == timer) {
    return false;
  }
  timer_reset(timer);
  linked_list_remove(timers, position);
  free(timer);
  timers_mark_updated();
  return true;
}

Timer* timers_find_last_wakeup(void) {
  Timer* last = NULL;
  uint32_t wakeup_time;
  uint16_t last_wakeup_time = 0;
  uint8_t count = timers_count();
  for (uint8_t c = 0; c < count; c += 1) {
    Timer* timer = timers_get(c);
    if (timer->wakeup_id < 0) {
      continue;
    }
    wakeup_time = timer_get_end_timestamp(timer);
    if (wakeup_time > last_wakeup_time) {
      last = timer;
      last_wakeup_time = wakeup_time;
    }
  }
  return last;
}

Timer* timers_find_wakeup_collision(Timer* timer) {
  time_t wakeup_time;
  wakeup_query(timer->wakeup_id, &wakeup_time);
  uint8_t count = timers_count();
  for (uint8_t c = 0; c < count; c += 1) {
    Timer* timer_to_check = timers_get(c);
    if (timer_to_check->wakeup_id < 0) {
      continue;
    }
    if (timer_to_check->id == timer->id) {
      continue;
    }
    time_t check_time;
    wakeup_query(timer_to_check->wakeup_id, &check_time);
    if (abs(check_time - wakeup_time) <= 60) {
      return timer_to_check;
    }
  }
  return NULL;
}

void timers_clear(void) {
  if (! timers) {
    return;
  }
  while (linked_list_count(timers) > 0) {
    Timer* timer = (Timer*) linked_list_get(timers, 0);
    linked_list_remove(timers, 0);
    free(timer);
  }
}

void timers_mark_updated(void) {
  if (queue_updates) {
    update_marked = true;
  } else {
    timers_fire_update_handlers();
    update_marked = false;
  }
}

void timers_fire_update_handlers(void) {
  uint8_t handler_count = linked_list_count(update_handlers);
  for (uint8_t h = 0; h < handler_count; h += 1) {
    TimersUpdatedHandler handler = linked_list_get(update_handlers, h);
    handler();
  }
}

void timers_highlight(Timer* timer) {
  uint8_t handler_count = linked_list_count(highlight_handlers);
  for (uint8_t h = 0; h < handler_count; h += 1) {
    TimerHighlightHandler handler = linked_list_get(highlight_handlers, h);
    handler(timer);
  }
}

void timers_register_update_handler(TimersUpdatedHandler handler) {
  linked_list_append(update_handlers, handler);
}

void timers_register_highlight_handler(TimerHighlightHandler handler) {
  linked_list_append(highlight_handlers, handler);
}

static TimerTimestamp new_timestamp() {
  return time(NULL);
}

void timers_update_timestamp(void) {
  current_time = new_timestamp();
  queue_updates = true;
  const uint8_t count = timers_count();
  for (uint8_t t = 0; t < count; ++t) {
    Timer* timer = timers_get(t);
    timer_tick(timer, current_time);
  }
  if (update_marked) {
    timers_fire_update_handlers();
    queue_updates = false;
  }
}

TimerTimestamp timers_current_timestamp() {
  return current_time;
}

static void timers_cleanup(void) {
  timers_clear();
  free(timers);
  timers = NULL;
}

void timers_save(void) {
  if (timers_count() == 0) {
    persist_delete(PERSIST_TIMER_START);
    return;
  }
  TimerBlock* block = NULL;
  uint8_t block_count = 0;
  for (uint8_t b = 0; b < timers_count(); b += 1) {
    if (NULL == block) {
      block = malloc(sizeof(TimerBlock));
      block->total_timers = timers_count();
      block->save_time = time(NULL);
    }

    uint8_t timer_block_pos = b % TIMER_BLOCK_SIZE;
    block->timers[timer_block_pos] = *timers_get(b);

    bool is_last_timer_in_block = timer_block_pos == (TIMER_BLOCK_SIZE - 1);
    if (is_last_timer_in_block) {
      persist_write_data(PERSIST_TIMER_START + block_count, block, sizeof(TimerBlock));
      block_count += 1;
      free(block);
      block = NULL;
    }
  }
  if (block) {
    persist_write_data(PERSIST_TIMER_START + block_count, block, sizeof(TimerBlock));
  }
  persist_write_int(PERSIST_TIMERS_VERSION, TIMERS_VERSION_CURRENT);
}

void timers_restore(void) {

  if (! persist_exists(PERSIST_TIMERS_VERSION)) {
    timers_migrate_1();
    return;
  }

  if (TIMERS_VERSION_TINY == persist_read_int(PERSIST_TIMERS_VERSION)) {
    timers_migrate_2();
    return;
  }

  if (TIMERS_VERSION_V3 == persist_read_int(PERSIST_TIMERS_VERSION)) {
    timers_migrate_3();
    return;
  }

  timers_clear();

  TimerBlock* block = NULL;
  if (persist_exists(PERSIST_TIMER_START)) {
    block = malloc(sizeof(TimerBlock));
    persist_read_data(PERSIST_TIMER_START, block, sizeof(TimerBlock));
    uint8_t num_timers = block->total_timers;
    uint8_t block_offset = 0;

    for (uint8_t t = 0; t < num_timers; t += 1) {
      if (! block) {
        block = malloc(sizeof(TimerBlock));
        persist_read_data(PERSIST_TIMER_START + block_offset, block, sizeof(TimerBlock));
      }
      Timer* timer = timer_clone(&block->timers[t % TIMER_BLOCK_SIZE]);
      timers_add(timer);
      timer_restore(timer, timers_current_timestamp());
      if (t % TIMER_BLOCK_SIZE == (TIMER_BLOCK_SIZE - 1)) {
        free(block);
        block = NULL;
        block_offset += 1;
      }
    }
    if (block) {
      free(block);
      block = NULL;
    }
  }
}

static void timers_migrate_1(void) {

  if (! persist_exists(PERSIST_TIMER_START)) {
    return;
  }

  int block = 0;
  OldTimerBlock* timerBlock = malloc(sizeof(OldTimerBlock));
  persist_read_data(PERSIST_TIMER_START, timerBlock, sizeof(OldTimerBlock));

  uint8_t timer_count = timerBlock->count;

  for (int t = 0; t < timer_count; t += 1) {

    if (t > 0 && t % TIMER_BLOCK_SIZE == 0) {
      block += 1;
      if (NULL != timerBlock) {
        free(timerBlock);
        timerBlock = NULL;
      }
      timerBlock = malloc(sizeof(OldTimerBlock));
      persist_read_data(PERSIST_TIMER_START + block, timerBlock, sizeof(OldTimerBlock));
    }

    OldTimer* old_timer = &timerBlock->timers[t % TIMER_BLOCK_SIZE];

    int seconds_elapsed = time(NULL) - timerBlock->time;

    Timer* timer = malloc(sizeof(Timer));
    timer->id = old_timer->id;
    timer->length = old_timer->length;
    timer->repeat = old_timer->repeat ? TIMER_REPEAT_INFINITE : 0;
    switch (old_timer->status) {
      case OLD_TIMER_STATUS_STOPPED:
        timer->status = TIMER_STATUS_STOPPED;
        break;
      case OLD_TIMER_STATUS_RUNNING:
        timer->status = TIMER_STATUS_RUNNING;
        break;
      case OLD_TIMER_STATUS_PAUSED:
        timer->status = TIMER_STATUS_PAUSED;
        break;
      case OLD_TIMER_STATUS_FINISHED:
        timer->status = TIMER_STATUS_DONE;
        break;
    }
    switch (old_timer->vibrate) {
      case OLD_TIMER_VIBRATION_OFF:
        timer->vibration = TIMER_VIBE_NONE;
        break;
      case OLD_TIMER_VIBRATION_SHORT:
        timer->vibration = TIMER_VIBE_SHORT;
        break;
      case OLD_TIMER_VIBRATION_LONG:
        timer->vibration = TIMER_VIBE_LONG;
        break;
      case OLD_TIMER_VIBRATION_DOUBLE:
        timer->vibration = TIMER_VIBE_DOUBLE;
        break;
      case OLD_TIMER_VIBRATION_TRIPLE:
        timer->vibration = TIMER_VIBE_TRIPLE;
        break;
      case OLD_TIMER_VIBRATION_CONTINUOUS:
        timer->vibration = TIMER_VIBE_SOLID;
        break;
    }
    switch (old_timer->direction) {
      case OLD_TIMER_DIRECTION_DOWN:
        timer->type = TIMER_TYPE_TIMER;
        break;
      case OLD_TIMER_DIRECTION_UP:
        timer->type = TIMER_TYPE_STOPWATCH;
        break;
    }
    timer->wakeup_id = -1;
    timer_restore_legacy(timer, timers_current_timestamp(), seconds_elapsed, old_timer->time_left);
    timers_add(timer);
  }
  if (NULL != timerBlock) {
    free(timerBlock);
  }
}

static void timers_migrate_2(void) {

  if (! persist_exists(PERSIST_TIMER_START)) {
    return;
  }

  int block_number = 0;
  TimerBlockTiny* block = malloc(sizeof(TimerBlockTiny));
  persist_read_data(PERSIST_TIMER_START, block, sizeof(TimerBlockTiny));

  uint8_t timer_count = block->total_timers;

  for (int t = 0; t < timer_count; t += 1) {

    if (t > 0 && t % TIMER_BLOCK_SIZE == 0) {
      block_number += 1;
      if (NULL != block) {
        free(block);
        block = NULL;
      }
      block = malloc(sizeof(TimerBlockTiny));
      persist_read_data(PERSIST_TIMER_START + block_number, block, sizeof(TimerBlockTiny));
    }

    TimerTiny* timer_tiny = &block->timers[t % TIMER_BLOCK_SIZE];

    int seconds_elapsed = time(NULL) - block->save_time;

    Timer* timer = malloc(sizeof(Timer));
    timer->id = timer_tiny->id;
    timer->length = timer_tiny->length;
    timer->repeat = timer_tiny->repeat;
    timer->repeat_count = timer_tiny->repeat_count;
    timer->status = timer_tiny->status;
    timer->vibration = timer_tiny->vibration;
    timer->type = timer_tiny->type;
    timer->wakeup_id = timer_tiny->wakeup_id;
    strcpy(timer->label, timer_tiny->label);
    timer_restore_legacy(timer, timers_current_timestamp(), seconds_elapsed, timer_tiny->current_time);
    timers_add(timer);
  }
  if (NULL != block) {
    free(block);
  }
}

static void timers_migrate_3(void) {

  if (! persist_exists(PERSIST_TIMER_START)) {
    return;
  }

  time_t now = time(NULL);
  uint16_t seconds_elapsed = 0;

  TimerBlockV3* block = NULL;
  if (persist_exists(PERSIST_TIMER_START)) {
    block = malloc(sizeof(TimerBlockV3));
    persist_read_data(PERSIST_TIMER_START, block, sizeof(TimerBlockV3));
    uint8_t num_timers = block->total_timers;
    uint8_t block_offset = 0;
    seconds_elapsed = now - block->save_time;

    for (uint8_t t = 0; t < num_timers; t += 1) {
      if (! block) {
        block = malloc(sizeof(TimerBlockV3));
        persist_read_data(PERSIST_TIMER_START + block_offset, block, sizeof(TimerBlockV3));
      }
      TimerV3* timer_v3 = &block->timers[t % TIMER_BLOCK_SIZE];
      Timer* timer = malloc(sizeof(Timer));

      timer->id = timer_v3->id;
      timer->type = timer_v3->type;
      timer->length = timer_v3->length;
      //timer->paused_offset is set by timer_restore_legacy
      //timer->start_timestamp is set by timer_restore_legacy
      timer->status = timer_v3->status;
      timer->vibration = timer_v3->vibration;
      timer->repeat = timer_v3->repeat;
      timer->repeat_count = timer_v3->repeat_count;
      timer->wakeup_id = timer_v3->wakeup_id;
      strncpy(timer->label, timer_v3->label, 24);
      timer_restore_legacy(timer, timers_current_timestamp(), seconds_elapsed, timer_v3->current_time);
      timers_add(timer);

      if (t % TIMER_BLOCK_SIZE == (TIMER_BLOCK_SIZE - 1)) {
        free(block);
        block = NULL;
        block_offset += 1;
      }
    }
    if (block) {
      free(block);
      block = NULL;
    }
  }
}
