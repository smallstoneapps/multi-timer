#include "pebble_os.h"
#include "timers.h"

Timer timers[MAX_TIMERS];
int num_timers = 0;

Timer* get_timer(int pos) {
  if (pos >= num_timers) {
    return NULL;
  }
  return &timers[pos];
}

int get_timer_count() {
  return num_timers;
}

void clear_timers() {
  num_timers = 0;
}

bool add_timer(int duration, TimerVibration vibrate) {
  if (duration <= 0) {
    return false;
  }
  if (num_timers >= MAX_TIMERS) {
    return false;
  }
  Timer t;
  t.length = duration;
  t.time_left = duration;
  t.status = TIMER_STOPPED;
  t.vibrate = vibrate;
  t.direction = TIMER_DOWN;
  timers[num_timers] = t;
  num_timers += 1;
  return true;
}

bool add_stopwatch() {
  if (num_timers >= MAX_TIMERS) {
    return false;
  }
  Timer t;
  t.time_left = 0;
  t.status = TIMER_STOPPED;
  t.direction = TIMER_UP;
  timers[num_timers] = t;
  num_timers += 1;
  return true;
}

void remove_timer(int pos) {
  if (pos >= num_timers) {
    return;
  }
  for (int p = pos + 1; p < num_timers; p += 1) {
    timers[p - 1] = timers[p];
  }
  num_timers -= 1;
}