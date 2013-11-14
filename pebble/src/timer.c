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
  timer->status = TIMER_STATUS_RUNNING;
  timer_set_app_timer(timer);
}

void timer_pause(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_PAUSED;
  timer_cancel_app_timer(timer);
}

void timer_resume(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_RUNNING;
  timer_set_app_timer(timer);
}

void timer_reset(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_STOPPED;
  timer->time_left = timer->length;
  timer_cancel_app_timer(timer);
}

void timer_tick(Timer* timer) {
  if (! timer) { return; }
  if (timer->status != TIMER_STATUS_RUNNING) { return; }
  if (timer->direction == TIMER_DIRECTION_DOWN) {
    timer->time_left -= 1;
    if (timer->time_left <= 0) {
      timer_finished(timer);
    }
  }
  else if (timer->direction == TIMER_DIRECTION_UP) {
    timer->time_left += 1;
  }
}

char* timer_vibe_str(TimerVibration vibe, bool shortStr) {
  switch (vibe) {
    case TIMER_VIBRATION_OFF:
      return "Off";
    case TIMER_VIBRATION_SHORT:
      return shortStr ? "Short" : "Short Pulse";
    case TIMER_VIBRATION_LONG:
      return shortStr ? "Long" : "Long Pulse";
    case TIMER_VIBRATION_DOUBLE:
      return shortStr ? "Double" : "Double Pulse";
    case TIMER_VIBRATION_TRIPLE:
      return shortStr ? "Triple" : "Triple Pulse";
    case TIMER_VIBRATION_CONTINUOUS:
      return shortStr ? "Solid" : "Continous";
  }
  return "";
}

char* timer_stringify(Timer* timer) {
  char* str = malloc(sizeof(char) * 32);
  if (timer->direction == TIMER_DIRECTION_UP) {
    snprintf(str, 20, "%d", timer->direction);
  }
  else {
    // Max length of string = 19 characters
    // X|XXXXX|X|X|XXXXX|X
    snprintf(str, 20, "%d|%d|%d|%d|%d|%d", timer->direction, timer->length, timer->vibrate, timer->repeat, timer->time_left, timer->status);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", str);
  }
  return str;
}

Timer* timer_unstringify(char* str) {
  Timer* timer = malloc(sizeof(Timer));
  if (strlen(str) == 1) {
    timer->direction = TIMER_DIRECTION_UP;
    return timer;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", str);

  timer->direction = TIMER_DIRECTION_DOWN;
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
  timer->vibrate = patoi(substr);
  start ++;
  end = start;

  while (*end != '\0' && *end != '|') {
    end ++;
  }
  strncpy(substr, start, (end - start));
  timer->time_left = patoi(substr);
  start ++;
  end = start;

  while (*end != '\0' && *end != '|') {
    end ++;
  }
  strncpy(substr, start, (end - start));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", substr);
  timer->status = patoi(substr);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %d", timer->status, TIMER_STATUS_RUNNING);

  return timer;
}

char* timer_describe(Timer* timer) {
  static char description[512];
  char type[16];
  char status[16];
  char repeat[16];
  char vibrate[16];

  switch (timer->direction) {
    case TIMER_DIRECTION_DOWN:
      strcpy(type, "countdown");
    break;
    case TIMER_DIRECTION_UP:
      strcpy(type, "stopwatch");
    break;
  }

  switch (timer->status) {
    case TIMER_STATUS_PAUSED:
      strcpy(status, "paused");
    break;
    case TIMER_STATUS_STOPPED:
      strcpy(status, "stopped");
    break;
    case TIMER_STATUS_RUNNING:
      strcpy(status, "running");
    break;
    case TIMER_STATUS_FINISHED:
      strcpy(status, "finished");
    break;
  }

  strcpy(repeat, timer->repeat ? "repeating" : "not repeating");

  strcpy(vibrate, timer_vibe_str(timer->vibrate, true));

  snprintf(description, 512, "I am a %s that is %s, length %d, %d left, %s, %s", type, status, timer->length, timer->time_left, repeat, vibrate);

  return description;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void timer_callback(void* data) {
  Timer* timer = (Timer*)data;
  timer->app_timer = NULL;

  if (timer->status != TIMER_STATUS_RUNNING) {
    return;
  }

  switch (timer->direction) {
    case TIMER_DIRECTION_UP:
      timer->time_left += 1;
    break;
    case TIMER_DIRECTION_DOWN:
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
  timer->status = TIMER_STATUS_FINISHED;
  switch (timer->vibrate) {
    case TIMER_VIBRATION_OFF:
      // Do nothing!
    break;
    case TIMER_VIBRATION_SHORT:
      vibes_short_pulse();
    break;
    case TIMER_VIBRATION_LONG:
      vibes_long_pulse();
    break;
    case TIMER_VIBRATION_DOUBLE: {
      const uint32_t seg[] = { 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBRATION_TRIPLE: {
      const uint32_t seg[] = { 600, 200, 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBRATION_CONTINUOUS:
      if (! win_vibrate_is_visible()) {
        win_vibrate_show();
      }
    break;
  }
  if (timer->repeat) {
    timer_start(timer);
  }
}
