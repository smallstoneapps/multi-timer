/*

Multi Timer 3.0
A Pebble watch app for running multiple simultaneous timers and stopwatches.
http://github.com/smallstoneapps/multi-timer/

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

tests/timers.c

*/

#include <pebble.h>
#include "unit.h"
#include "groups.h"
#include "../src/timers.h"
#include "../src/migration.h"
#include "../src/persist.h"

void timers_before(void) {
  timers_init();
}

char* count_zero_when_empty(void) {
  mu_assert(0 == timers_count(), "timers_count was not 0 when no timers added");
  return 0;
}

char* count_one_when_added_single_timer(void) {
  Timer* timer = malloc(sizeof(Timer));
  timers_add(timer);
  mu_assert(1 == timers_count(), "timers_count was not 1 when a timer was added");
  return 0;
}

char* count_10_when_added_ten_timers(void) {
  for (uint8_t t = 0; t < 10; t += 1) {
    Timer* timer = malloc(sizeof(Timer));
    timers_add(timer);
  }
  mu_assert(10 == timers_count(), "timers_count was not 10 when ten timers were added");
  return 0;
}

char* get_timer_null_when_empty(void) {
  mu_assert(NULL == timers_get(0), "timers_get was not null when empty at position 0");
  mu_assert(NULL == timers_get(100), "timers_get was not null when empty at position 100");
  return 0;
}

char* get_timer_returns_item(void) {
  Timer* timer = malloc(sizeof(Timer));
  timers_add(timer);
  mu_assert(NULL != timers_get(0), "timers_get was null with one item at position 0");
  mu_assert(NULL == timers_get(1), "timers_get was not null with one item at position 1");
  return 0;
}

char* find_null_when_empty(void) {
  mu_assert(NULL == timers_find(1), "timers_find was not null when empty with id 1");
  mu_assert(NULL == timers_find(8888), "timers_get was not null when empty with id 8888");
  return 0;
}

char* find_timer_when_exists(void) {
  Timer* timer = malloc(sizeof(Timer));
  timer->id = 1;
  timers_add(timer);
  mu_assert(NULL != timers_find(1), "timers_find was null when exists with id 1");
  return 0;
}

char* timer_remove_false_when_empty(void) {
  mu_assert(false == timers_remove(0), "timers_remove failed when empty");
  return 0;
}

char* timer_remove_true_when_single_timer(void) {
  Timer* timer = malloc(sizeof(Timer));
  timers_add(timer);
  mu_assert(true == timers_remove(0), "timers_remvoe succeeds when single timer");
  return 0;
}

char* timer_remove_false_when_not_existant(void) {
  Timer* timer = malloc(sizeof(Timer));
  timers_add(timer);
  mu_assert(false == timers_remove(5), "timers_remvoe fails when invalid position");
  return 0;
}

char* timer_remove_reduces_count(void) {
  timers_add(malloc(sizeof(Timer)));
  timers_add(malloc(sizeof(Timer)));
  timers_remove(0);
  mu_assert(1 == timers_count(), "timers_remove did not reduce timers count");
  return 0;
}

char* timers_save_persists_zero_timers(void) {
  timers_save();
  timers_clear();
  timers_restore();
  mu_assert(0 == timers_count(), "timers_save with no timers did not restore to zero timers");
  return 0;
}


char* timers_save_persists_zero_timers_after_lots(void) {
  timers_add(malloc(sizeof(Timer)));
  timers_add(malloc(sizeof(Timer)));
  timers_save();
  timers_clear();
  timers_save();
  timers_restore();
  mu_assert(0 == timers_count(), "timers_save with no timers (after lots) did not restore to zero timers");
  return 0;
}


char* timers_save_persists_one_timer(void) {
  Timer* timer = malloc(sizeof(Timer));
  timer->id = 10;
  timers_add(timer);
  timers_save();
  timers_clear();
  timers_restore();
  mu_assert(1 == timers_count(), "timers_save with one timer did not restore a timer");
  mu_assert(timer->id == timers_get(0)->id, "timers_save with one timer did not restore the timer correctly");
  return 0;
}

char* timers_save_persists_multiple_timers(void) {
  Timer* timer1 = malloc(sizeof(Timer));
  timer1->id = 10;
  timers_add(timer_clone(timer1));

  Timer* timer2 = malloc(sizeof(Timer));
  timer2->id = 20;
  timers_add(timer_clone(timer2));

  Timer* timer3 = malloc(sizeof(Timer));
  timer3->id = 30;
  timers_add(timer_clone(timer3));

  Timer* timer4 = malloc(sizeof(Timer));
  timer4->id = 40;
  timers_add(timer_clone(timer4));

  Timer* timer5 = malloc(sizeof(Timer));
  timer5->id = 50;
  timers_add(timer_clone(timer5));

  timers_save();
  timers_clear();
  timers_restore();
  mu_assert(5 == timers_count(), "timers_save with multiple timer did not restore the timers");
  mu_assert(timer1->id == timers_get(0)->id, "timers_save with multiple timer did not restore timer 1 correctly");
  mu_assert(timer2->id == timers_get(1)->id, "timers_save with multiple timer did not restore timer 2 correctly");
  mu_assert(timer3->id == timers_get(2)->id, "timers_save with multiple timer did not restore timer 3 correctly");
  mu_assert(timer4->id == timers_get(3)->id, "timers_save with multiple timer did not restore timer 4 correctly");
  mu_assert(timer5->id == timers_get(4)->id, "timers_save with multiple timer did not restore timer 5 correctly");
  return 0;
}

char* timer_str(void) {
  Timer* timer = malloc(sizeof(Timer));
  char str[16];

  timer_time_str(0, false, str, 16);
  mu_assert(0 == strcmp("00:00", str), "timer_str did not display 0 correctly (no hours)");

  timer_time_str(60, false, str, 16);
  mu_assert(0 == strcmp("01:00", str), "timer_str did not display 60 correctly (no hours)");

  timer_time_str(3600, false, str, 16);
  mu_assert(0 == strcmp("60:00", str), "timer_str did not display 3600 correctly (no hours)");

  timer_time_str(3600, true, str, 16);
  mu_assert(0 == strcmp("01:00:00", str), "timer_str did not display 3600 correctly (hours)");

  return 0;
}

char* timers_migrate_from_1_to_3(void) {
  OldTimerBlock* block = malloc(sizeof(OldTimerBlock));
  block->count = 2;
  block->time = time(NULL);
  block->timers[0].direction = OLD_TIMER_DIRECTION_DOWN;
  block->timers[0].length = 3000;
  block->timers[0].time_left = 200;
  block->timers[0].status = OLD_TIMER_STATUS_PAUSED;
  block->timers[0].repeat = true;
  block->timers[0].vibrate = OLD_TIMER_VIBRATION_DOUBLE;
  block->timers[1].direction = OLD_TIMER_DIRECTION_UP;
  block->timers[1].time_left = 50;
  block->timers[1].status = OLD_TIMER_STATUS_RUNNING;
  persist_write_data(PERSIST_TIMER_START, block, sizeof(OldTimerBlock));
  free(block);

  timers_restore();
  mu_assert(2 == timers_count(), "Migrate 1->3: Incorrect number of timers");

  Timer* tmr1 = timers_get(0);
  Timer* tmr2 = timers_get(1);

  mu_assert(tmr1->type == TIMER_TYPE_TIMER, "Migrate 1->3: Timer 1 wrong type");
  mu_assert(tmr1->length == 3000, "Migrate 1->3: Timer 1 wrong length");
  mu_assert(tmr1->current_time == 200, "Migrate 1->3: Timer 1 wrong time");
  mu_assert(tmr1->status == TIMER_STATUS_PAUSED, "Migrate 1->3: Timer 1 wrong status");
  mu_assert(tmr1->repeat == TIMER_REPEAT_INFINITE, "Migrate 1->3: Timer 1 wrong repeat");
  mu_assert(tmr1->vibration == TIMER_VIBE_DOUBLE , "Migrate 1->3: Timer 1 wrong vibration");
  mu_assert(tmr2->type == TIMER_TYPE_STOPWATCH , "Migrate 1->3: Timer 2 wrong type");
  mu_assert(tmr2->current_time == 50, "Migrate 1->3: Timer 2 wrong time");
  mu_assert(tmr2->status == TIMER_STATUS_RUNNING , "Migrate 1->3: Timer 2 wrong status");

  return 0;
}

char* timers_migrate_from_2_to_3(void) {
  TimerBlockTiny* block = malloc(sizeof(TimerBlockTiny));
  block->total_timers = 2;
  block->save_time = time(NULL);
  block->timers[0].type = TIMER_TYPE_TIMER;
  block->timers[0].length = 60;
  block->timers[0].current_time = 45;
  block->timers[0].id = 5;
  strcpy(block->timers[0].label, "LABEL #1");
  block->timers[0].repeat = TIMER_REPEAT_INFINITE;
  block->timers[0].vibration = TIMER_VIBE_SOLID;
  block->timers[0].status = TIMER_STATUS_PAUSED;
  block->timers[0].wakeup_id = 70;
  block->timers[1].type = TIMER_TYPE_STOPWATCH;
  block->timers[1].current_time = 500;
  block->timers[1].id = 10;
  strcpy(block->timers[1].label, "LABEL #2");
  block->timers[1].status = TIMER_STATUS_RUNNING;
  block->timers[1].wakeup_id = 60;

  persist_write_data(PERSIST_TIMER_START, block, sizeof(TimerBlockTiny));
  persist_write_int(PERSIST_TIMERS_VERSION, TIMERS_VERSION_TINY);
  free(block);

  timers_restore();
  mu_assert(2 == timers_count(), "Migrate 2->3: Incorrect number of timers");

  Timer* tmr1 = timers_get(0);
  Timer* tmr2 = timers_get(1);

  mu_assert(tmr1->type == TIMER_TYPE_TIMER, "Migrate 2->3: Timer 1 wrong type");
  mu_assert(tmr1->length == 60, "Migrate 2->3: Timer 1 wrong length");
  mu_assert(tmr1->current_time == 45, "Migrate 2->3: Timer 1 wrong current time");
  mu_assert(tmr1->id == 5, "Migrate 2->3: Timer 1 wrong ID");
  mu_assert(0 == strcmp(tmr1->label, "LABEL #1"), "Migrate 2->3: Timer 1 wrong label");
  mu_assert(tmr1->repeat == TIMER_REPEAT_INFINITE, "Migrate 2->3: Timer 1 wrong repeat");
  mu_assert(tmr1->vibration == TIMER_VIBE_SOLID, "Migrate 2->3: Timer 1 wrong vibration");
  mu_assert(tmr1->status == TIMER_STATUS_PAUSED, "Migrate 2->3: Timer 1 wrong status");
  mu_assert(tmr1->wakeup_id == 70, "Migrate 2->3: Timer 1 wrong wakeup ID");

  mu_assert(tmr2->type == TIMER_TYPE_STOPWATCH, "Migrate 2->3: Timer 2 wrong wakeup type");
  mu_assert(tmr2->current_time == 500, "Migrate 2->3: Timer 2 wrong current time");
  mu_assert(tmr2->id == 10, "Migrate 2->3: Timer 2 wrong id");
  mu_assert(0 == strcmp(tmr2->label, "LABEL #2"), "Migrate 2->3: Timer 2 wrong label");
  mu_assert(tmr2->status == TIMER_STATUS_RUNNING, "Migrate 2->3: Timer 2 wrong status");
  mu_assert(tmr2->wakeup_id == 60, "Migrate 2->3: Timer 2 wrong wakeup ID");

  return 0;
}

char* timers_tests(void) {
  mu_run_test(count_zero_when_empty);
  mu_run_test(count_one_when_added_single_timer);
  mu_run_test(count_10_when_added_ten_timers);
  mu_run_test(get_timer_null_when_empty);
  mu_run_test(get_timer_returns_item);
  mu_run_test(find_null_when_empty);
  mu_run_test(find_timer_when_exists);
  mu_run_test(timer_remove_false_when_empty);
  mu_run_test(timer_remove_true_when_single_timer);
  mu_run_test(timer_remove_false_when_not_existant);
  mu_run_test(timer_remove_reduces_count);
  mu_run_test(timers_save_persists_zero_timers);
  mu_run_test(timers_save_persists_zero_timers_after_lots);
  mu_run_test(timers_save_persists_one_timer);
  mu_run_test(timers_save_persists_multiple_timers);
  mu_run_test(timer_str);
  mu_run_test(timers_migrate_from_1_to_3);
  mu_run_test(timers_migrate_from_2_to_3);
  return 0;
}
