/*

Multi Timer v2.7.1
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

tests/timers.c

*/

#include "include/pebble_extra.h"
#include "unit.h"
#include "../src/timers.h"
#include "../src/settings.h"
#include "../src/generated/appinfo.h"

// Colour code definitions to make the output all pretty.
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Keep track of how many tests have run, and how many have passed.
int tests_run = 0;
int tests_passed = 0;
const int NUM_TESTS = 8;

static void before_each(void) {
  timers_init();
  persist_init();
}

static void after_each(void) {
  persist_reset();
}

// Helper function that creates a new down timer.
static Timer* create_timer_down(uint16_t duration) {
  Timer* tmr = malloc(sizeof(Timer));
  tmr->id = rand() % 1000;
  tmr->direction = TIMER_DIRECTION_DOWN;
  tmr->length = duration;
  tmr->vibrate = TIMER_VIBRATION_SHORT;
  tmr->time_left = duration;
  tmr->label[0] = 0;
  tmr->repeat = false;
  tmr->status = TIMER_STATUS_STOPPED;
  return tmr;
}

// Helper function that creates a new up timer.
static Timer* create_timer_up(void) {
  Timer* tmr = malloc(sizeof(Timer));
  tmr->id = rand() % 1000;
  tmr->label[0] = 0;
  tmr->direction = TIMER_DIRECTION_UP;
  tmr->length = 0;
  tmr->vibrate = TIMER_VIBRATION_SHORT;
  tmr->time_left = 0;
  tmr->repeat = false;
  tmr->status = TIMER_STATUS_STOPPED;
  return tmr;
}

// Helper function that returns true if the timers are identical.
static bool compare_timers(Timer* left, Timer* right) {
  if (left->direction != right->direction) { return false; }
  if (left->id != right->id) { return false; }
  if (left->length != right->length) { return false; }
  if (left->time_left != right->time_left) { return false; }
  if (left->vibrate != right->vibrate) { return false; }
  if (left->repeat != right->repeat) { return false; }
  if (0 != strcmp(left->label, right->label)) { return false; }
  return true;
}

// When no timers have been added
// The timer count should be 0
static char* test_empty(void) {
  bool pass = (0 == timers_get_count());
  mu_assert(pass, "Timers reporting not empty.");
  return 0;
}

// When a single timer has been added
// The timer count should be 1.
static char* test_one(void) {
  timers_add(create_timer_down(60));
  mu_assert(1 == timers_get_count(), "Timers not reporting size of 1");
  return 0;
}

// When a timer has been added
// And a timer has been removed
// The timer count should be 0.
static char* test_add_remove_one(void) {
  Timer* t = malloc(sizeof(Timer));
  timers_add(t);
  timers_remove(0);
  mu_assert(0 == timers_get_count(), "Timers not reporting size of 0");
  return 0;
}

// When 100 timers are added
// And 100 timers are removed
// The timer count should be 0.
static char* test_fill_empty(void) {
  const int num_timers = 100;
  for (int t = 0; t < num_timers; t += 1) {
    Timer* t = malloc(sizeof(Timer));
    timers_add(t);
  }
  for (int t = (num_timers - 1); t >= 0; t -= 1) {
    timers_remove(t);
  }
  mu_assert(0 == timers_get_count(), "Timers not reporting size of 0");
  return 0;
}

// When a timer is added
// And a invalid index is removed
// The timer count should be 1.
static char* test_remove_missing(void) {
  timers_remove(0);
  timers_add(malloc(sizeof(Timer)));
  timers_remove(100);
  mu_assert(1 == timers_get_count(), "Attempting to remove timers that don't exist caused error.");
  return 0;
}

// When a timer is added
// The timer retrieved should match the one added.
static char* test_get_timers(void) {
  Timer* none1 = timers_get(0);
  mu_assert(NULL == none1, "Get on empty wasn't NULL");
  Timer* tmr = malloc(sizeof(Timer));
  timers_add(tmr);
  Timer* get = timers_get(0);
  mu_assert(compare_timers(tmr, get), "Get timer doesn't match");
  Timer* none2 = timers_get(1);
  mu_assert(NULL == none1, "Get on invalid position wasn't NULL");
  return 0;
}

// When there are no timers.
// And the timers are saved
// And the timers are loaded
// The timers count should be 0.
static char* test_persist_none(void) {
  mu_assert(0 == timers_get_count(), "test_persist_none() failed to have no timers [1].");
  status_t status = timers_save();
  mu_assert(0 >= status, "test_persist_none() failed to save timers");
  timers_clear();
  timers_restore();
  mu_assert(0 == timers_get_count(), "test_persist_none() failed to have no timers [2].");
  return 0;
}

// When a timer has been added.
// And the timers are saved
// And the timers are loaded
// The timers count should be 0.
// And the timer retrieved should match the one added.
static char* test_persist_one(void) {
  Timer* tmr = create_timer_down(10 * 60);
  timers_add(tmr);
  mu_assert(1 == timers_get_count(), "test_persist_one() didn't correctly save a timer.");
  timers_save();
  timers_clear();
  mu_assert(0 == timers_get_count(), "test_persist_one() didn't correctly clear the timers.");
  timers_restore();
  mu_assert(1 == timers_get_count(), "test_persist_one() didn't correctly restore the timers.");
  mu_assert(compare_timers(timers_get(0), tmr), "test_persist_one() didn't load the timer correctly.");
  return 0;
}

// When 6 timers has been added.
// And the timers are saved
// And the timers are loaded
// The timers count should be 6
// And the timers retrieved should match the ones added.
static char* test_persist_multiple(void) {
  Timer* tmr1 = create_timer_down(10 * 60);
  Timer* tmr2 = create_timer_up();
  Timer* tmr3 = create_timer_down(5 * 60);
  Timer* tmr4 = create_timer_down(1 * 60);
  Timer* tmr5 = create_timer_down(2 * 60);
  Timer* tmr6 = create_timer_down(3 * 60);
  timers_add(timer_clone(tmr1));
  timers_add(timer_clone(tmr2));
  timers_add(timer_clone(tmr3));
  timers_add(timer_clone(tmr4));
  timers_add(timer_clone(tmr5));
  timers_add(timer_clone(tmr6));
  mu_assert(6 == timers_get_count(), "test_persist_multiple() didn't correctly save the timers.");
  timers_save();
  timers_clear();
  mu_assert(0 == timers_get_count(), "test_persist_multiple() didn't correctly clear the timers.");
  timers_restore();
  mu_assert(6 == timers_get_count(), "test_persist_multiple() didn't correctly restore the timers.");
  bool pass = compare_timers(timers_get(0), tmr1);
  pass = pass && compare_timers(timers_get(1), tmr2);
  pass = pass && compare_timers(timers_get(2), tmr3);
  pass = pass && compare_timers(timers_get(3), tmr4);
  pass = pass && compare_timers(timers_get(4), tmr5);
  pass = pass && compare_timers(timers_get(5), tmr6);
  mu_assert(pass, "test_persist_multiple() didn't load the timers correctly.");
  return 0;
}

// When a timer has been added
// And the timers are saved
// And the timers are cleared
// And the timers are saved
// And the timers are loaded
// The timers count should be 0.
static char* test_persist_after_clear(void) {
  timers_add(create_timer_down(10 * 60));
  timers_save();
  timers_clear();
  timers_save();
  timers_restore();
  mu_assert(0 == timers_get_count(), "Timers exist when none should.");
  return 0;
}

// When a timer is added
// And the timer is running
// And the timers are saved and loaded
// And the "auto resume" setting is enable
// The timer should still be running
static char* test_resume_running_timer(void) {
  settings()->resume_timers = true;
  mu_assert(settings()->resume_timers == true, "Resume timers setting is not on.");
  timers_add(create_timer_down(2 * 60));
  Timer* tmr = timers_get(0);
  timer_resume(tmr);
  mu_assert(TIMER_STATUS_RUNNING == tmr->status, "Created timer is not running.");
  timers_save();
  timers_clear();
  mu_assert(0 == timers_get_count(), "Sanity check that timers are gone.");
  timers_restore();
  tmr = timers_get(0);
  mu_assert(TIMER_STATUS_RUNNING == tmr->status, "Loaded timer is not running.");
  return 0;
}

// When a timer is added
// And the timer is stopped
// And the timers are saved and loaded
// And the "auto resume" setting is enable
// The timer should not be running
static char* test_resume_stopped_timer(void) {
  mu_assert(settings()->resume_timers == true, "Resume timers setting is not on.");
  timers_add(create_timer_down(2 * 60));
  Timer* tmr = timers_get(0);
  timer_reset(tmr);
  mu_assert(TIMER_STATUS_STOPPED == tmr->status, "Created timer is not stopped.");
  timers_save();
  timers_clear();
  mu_assert(0 == timers_get_count(), "Sanity check that timers are gone.");
  timers_restore();
  tmr = timers_get(0);
  mu_assert(TIMER_STATUS_STOPPED == tmr->status, "Loaded timer is not stopped.");
  return 0;
}

// When a timer is added
// And the timer is running
// And the timers are saved and loaded
// And the "auto resume" setting is disabled
// The timer should still be running
static char* test_dont_resume_running_timer(void) {
  settings()->resume_timers = false;
  mu_assert(settings()->resume_timers == false, "Resume timers setting is not off.");
  timers_add(create_timer_down(2 * 60));
  Timer* tmr = timers_get(0);
  timer_resume(tmr);
  mu_assert(TIMER_STATUS_RUNNING == tmr->status, "Created timer is not running.");
  timers_save();
  timers_clear();
  mu_assert(0 == timers_get_count(), "Sanity check that timers are gone.");
  timers_restore();
  tmr = timers_get(0);
  mu_assert(TIMER_STATUS_STOPPED == tmr->status, "Loaded timer is not stopped.");
  return 0;
}

// When a timer is added
// And we search for it by id
// The timer should be returned
static char* test_find_valid(void) {
  Timer* tmr = create_timer_down(50);
  timers_add(tmr);
  mu_assert(timers_find(tmr->id)->id == tmr->id, "Could not find timer by id");
  return 0;
}

// Run all the tests!
static char* all_tests() {
  mu_run_test(test_empty);
  mu_run_test(test_one);
  mu_run_test(test_add_remove_one);
  mu_run_test(test_fill_empty);
  mu_run_test(test_remove_missing);
  mu_run_test(test_get_timers);
  mu_run_test(test_persist_none);
  mu_run_test(test_persist_one);
  mu_run_test(test_persist_multiple);
  mu_run_test(test_persist_after_clear);
  mu_run_test(test_resume_running_timer);
  mu_run_test(test_resume_stopped_timer);
  mu_run_test(test_dont_resume_running_timer);
  mu_run_test(test_find_valid);
  return 0;
}

// Test application entry point.
// Executes all the tests and prints the results in pretty colours.
int main(int argc, char **argv) {
  printf("%s-----------------------------------\n", KCYN);
  printf("| Running Multi Timer %s Tests |\n", VERSION_LABEL);
  printf("-----------------------------------\n%s", KNRM);
  char* result = all_tests();
  if (0 != result) {
    printf("%sFailed Test:%s %s\n", KRED, KNRM, result);
  }
  printf("Tests Run: %s%d%s\n", (tests_run == tests_passed) ? KGRN : KRED, tests_run, KNRM);
  printf("Tests Passed: %s%d%s\n", (tests_run == tests_passed) ? KGRN : KRED, tests_passed, KNRM);

  printf("%s-----------------------------------%s\n", KCYN, KNRM);
  return result != 0;
}