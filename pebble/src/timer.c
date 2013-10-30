/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timer.c
 ***/

#include <pebble_os.h>
#include "win-timers.h"
#include "timer.h"

static void timer_callback(void* data);
static void timer_set_app_timer(Timer* timer);
static void timer_cancel_app_timer(Timer* timer);
static void timer_finished(Timer* timer);

void timer_start(Timer* timer) {
  if (! timer) { return; }
  timer->time_left = timer->length;
  timer->status = TIMER_RUNNING;
  timer_set_app_timer(timer);
}

void timer_pause(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_PAUSED;
  timer_cancel_app_timer(timer);
}

void timer_resume(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_RUNNING;
  timer_set_app_timer(timer);
}

void timer_reset(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STOPPED;
  timer->time_left = timer->length;
  timer_cancel_app_timer(timer);
}

void timer_tick(Timer* timer) {
  if (! timer) { return; }
  if (timer->status != TIMER_RUNNING) { return; }
  if (timer->direction == TIMER_DOWN) {
    timer->time_left -= 1;
    if (timer->time_left <= 0) {
      timer_finished(timer);
    }
  }
  else if (timer->direction == TIMER_UP) {
    timer->time_left += 1;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void timer_callback(void* data) {
  Timer* timer = (Timer*)data;

  if (timer->status != TIMER_RUNNING) {
    return;
  }

  switch (timer->direction) {
    case TIMER_UP:
      timer->time_left += 1;
    break;
    case TIMER_DOWN:
      timer->time_left -= 1;
      if (timer->time_left == 0) {
        timer_finished(timer);
      }
    break;
  }

  timer_set_app_timer(timer);
  win_timers_update();
}

static void timer_set_app_timer(Timer* timer) {
  timer_cancel_app_timer(timer);
  timer->app_timer = app_timer_register(1000, timer_callback, (void*)timer);
}

static void timer_cancel_app_timer(Timer* timer) {
  if (timer->app_timer != NULL) {
    app_timer_cancel(timer->app_timer);
    timer->app_timer = NULL;
  }
}

static void timer_finished(Timer* timer) {
  timer->status = TIMER_FINISHED;
  switch (timer->vibrate) {
    case TIMER_VIBE_OFF:
      // Do nothing!
    break;
    case TIMER_VIBE_SHORT:
      vibes_short_pulse();
    break;
    case TIMER_VIBE_LONG:
      vibes_long_pulse();
    break;
    case TIMER_VIBE_DOUBLE: {
      const uint32_t seg[] = { 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBE_TRIPLE: {
      const uint32_t seg[] = { 600, 200, 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBE_CONTINUOUS:
      // TODO: Implement this.
    break;
  }
  if (timer->repeat) {
    timer_start(timer);
  }
}
