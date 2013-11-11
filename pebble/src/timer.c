/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timer.c
 ***/

#include <pebble.h>
#include "windows/win-timers.h"
#include "windows/win-vibrate.h"
#include "libs/pebble-assist.h"
#include "common.h"
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

char* timer_vibe_str(TimerVibration vibe, bool shortStr) {
  switch (vibe) {
    case TIMER_VIBE_OFF:
      return "Off";
    case TIMER_VIBE_SHORT:
      return shortStr ? "Short" : "Short Pulse";
    case TIMER_VIBE_LONG:
      return shortStr ? "Long" : "Long Pulse";
    case TIMER_VIBE_DOUBLE:
      return shortStr ? "Double" : "Double Pulse";
    case TIMER_VIBE_TRIPLE:
      return shortStr ? "Triple" : "Triple Pulse";
    case TIMER_VIBE_CONTINUOUS:
      return shortStr ? "Solid" : "Continous";
  }
  return "";
}

char* timer_stringify(Timer* timer) {
  char* str = malloc(sizeof(char) * 32);
  if (timer->direction == TIMER_UP) {
    snprintf(str, 32, "%d", timer->direction);
  }
  else {
    snprintf(str, 32, "%d|%d|%d|%d", timer->direction, timer->length, timer->vibrate, timer->repeat);
  }
  return str;
}

Timer* timer_unstringify(char* str) {
  Timer* timer = malloc(sizeof(Timer));
  if (strlen(str) == 1) {
    timer->direction = TIMER_UP;
    return timer;
  }

  timer->direction = TIMER_DOWN;
  char* start = str;
  start ++; start ++;
  char* end = start;
  char substr[8] = "";

  while (*end != '\0' && *end != '|') {
    end ++;
  }
  strncpy(substr, start, (end - start));
  timer->length = patoi(substr);
  start ++;
  end = start;

  while (*end != '\0' && *end != '|') {
    end ++;
  }
  strncpy(substr, start, (end - start));
  timer->vibrate = patoi(substr);
  start ++;
  end = start;

  while (*end != '\0' && *end != '|') {
    end ++;
  }
  strncpy(substr, start, (end - start));
  timer->repeat = patoi(substr);

  return timer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void timer_callback(void* data) {
  Timer* timer = (Timer*)data;
  timer->app_timer = NULL;

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
      if (! win_vibrate_is_visible()) {
        win_vibrate_show();
      }
    break;
  }
  if (timer->repeat) {
    timer_start(timer);
  }
}
