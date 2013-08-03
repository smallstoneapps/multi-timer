#include "pebble_app.h"
#include "timer.h"

void timer_start(Timer* timer) {
  if (! timer) { return; }
  timer->time_left = timer->length;
  timer->status = TIMER_RUNNING;
}

void timer_pause(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_PAUSED;
}

void timer_resume(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_RUNNING;
}

void timer_reset(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STOPPED;
  timer->time_left = timer->length;
}

void timer_tick(Timer* timer) {
  if (! timer) { return; }
  if (timer->status != TIMER_RUNNING) { return; }
  if (timer->direction == TIMER_DOWN) {
    timer->time_left -= 1;
    if (timer->time_left <= 0) {
      timer->status = TIMER_FINISHED;
      if (timer->vibrate) {
        vibes_long_pulse();
      }
    }
  }
  else if (timer->direction == TIMER_UP) {
    timer->time_left += 1;
  }
}