#include "unit.h"
#include "../src/timers.h"

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
const int NUM_TESTS = 6;

static void before_each(void) {
  timers_clear();
}

static void after_each(void) {
}

static char* test_empty(void) {
  bool pass = (0 == timers_get_count());
  mu_assert(pass, "Timers reporting not empty.");
  return 0;
}

static char* test_one(void) {
  Timer* t = malloc(sizeof(Timer));
  timers_add(t);
  mu_assert(1 == timers_get_count(), "Timers not reporting size of 1");
  return 0;
}

static char* test_add_remove_one(void) {
  Timer* t = malloc(sizeof(Timer));
  timers_add(t);
  timers_remove(0);
  mu_assert(0 == timers_get_count(), "Timers not reporting size of 0");
  return 0;
}

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

static char* test_remove_missing(void) {
  timers_remove(0);
  timers_add(malloc(sizeof(Timer)));
  timers_remove(100);
  mu_assert(1 == timers_get_count(), "Attempting to remove timers that don't exist caused error.");
  return 0;
}

static char* test_get_timers(void) {
  Timer* none1 = timers_get(0);
  mu_assert(NULL == none1, "Get on empty wasn't NULL");
  Timer* tmr = malloc(sizeof(Timer));
  timers_add(tmr);
  Timer* get = timers_get(0);
  mu_assert(tmr == get, "Get timer doesn't match");
  Timer* none2 = timers_get(1);
  mu_assert(NULL == none1, "Get on invalid position wasn't NULL");
  return 0;
}

static char* all_tests() {
  mu_run_test(test_empty);
  mu_run_test(test_one);
  mu_run_test(test_add_remove_one);
  mu_run_test(test_fill_empty);
  mu_run_test(test_remove_missing);
  mu_run_test(test_get_timers);
  return 0;
}

int main(int argc, char **argv) {
  printf("%s----------------------------\n", KCYN);
  printf("Running Multi Timer Tests\n");
  printf("----------------------------\n%s", KNRM);
  char* result = all_tests();
  if (0 != result) {
    printf("%sFailed Test:%s %s\n", KRED, KNRM, result);
  }
  printf("Tests Run: %s%d / %d%s\n", (tests_run == NUM_TESTS) ? KGRN : KRED, tests_run, NUM_TESTS, KNRM);
  printf("Tests Passed: %s%d / %d%s\n", (tests_passed == NUM_TESTS) ? KGRN : KRED, tests_passed, NUM_TESTS, KNRM);

  printf("%s----------------------------%s\n", KCYN, KNRM);
  return result != 0;
}