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

src/timer.c

*/

#include <pebble.h>
#include "timers.h"
#include "timer.h"
#include "icons.h"
#include "settings.h"
#include "windows/win-vibrate.h"

static void timer_tick(void* context);
static void timer_finish(Timer* timer);
static void timer_schedule_tick(Timer* timer);
static void timer_cancel_tick(Timer* timer);
static void timer_schedule_wakeup(Timer* timer, uint16_t offset);
static void timer_cancel_wakeup(Timer* timer);
static void timer_set_id(Timer* timer);
static void timer_completed_action(Timer* timer);

void timer_time_str(uint32_t timer_time, bool showHours, char* str, int str_len) {
  int hours = timer_time / 3600;
  int minutes = (showHours ? (timer_time % 3600) : timer_time) / 60;
  int seconds = (showHours ? (timer_time % 3600) : timer_time) % 60;
  if (showHours) {
    snprintf(str, str_len, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  else {
    snprintf(str, str_len, "%02d:%02d", minutes, seconds);
  }
}

void timer_start(Timer* timer) {
  switch (timer->type) {
    case TIMER_TYPE_TIMER:
      timer->current_time = timer->length;
      break;
    case TIMER_TYPE_STOPWATCH:
      timer->current_time = 0;
      break;
  }
  timer->status = TIMER_STATUS_RUNNING;
  timer_schedule_tick(timer);
  timer_schedule_wakeup(timer, 0);
  timers_mark_updated();
}

void timer_pause(Timer* timer) {
  timer->status = TIMER_STATUS_PAUSED;
  timer_cancel_tick(timer);
  timer_cancel_wakeup(timer);
  timers_mark_updated();
}

void timer_resume(Timer* timer) {
  timer->status = TIMER_STATUS_RUNNING;
  timer_schedule_tick(timer);
  timer_schedule_wakeup(timer, 0);
  timers_mark_updated();
}

void timer_reset(Timer* timer) {
  timer_pause(timer);
  switch (timer->type) {
    case TIMER_TYPE_TIMER:
      timer->current_time = timer->length;
      break;
    case TIMER_TYPE_STOPWATCH:
      timer->current_time = 0;
      break;
  }
  timer->status = TIMER_STATUS_STOPPED;
  timers_mark_updated();
}

void timer_restore(Timer* timer, uint16_t seconds_elapsed) {
  timer->timer = NULL;
  if (timer->status == TIMER_STATUS_RUNNING) {
    switch (timer->type) {
      case TIMER_TYPE_STOPWATCH:
        timer->current_time += seconds_elapsed;
        break;
      case TIMER_TYPE_TIMER: {
        if (seconds_elapsed >= timer->current_time) {
          timer->current_time = 0;
          timer->status = TIMER_STATUS_DONE;
        }
        else {
          timer->current_time -= seconds_elapsed;
        }
        break;
      }
    }
  }
  if (timer->status == TIMER_STATUS_RUNNING) {
    timer_resume(timer);
  }
}

Timer* timer_clone(Timer* timer) {
  Timer* new_timer = malloc(sizeof(Timer));
  memcpy(new_timer, timer, sizeof(Timer));
  return new_timer;
}

char* timer_vibe_str(TimerVibration vibe, bool shortStr) {
  switch (vibe) {
    case TIMER_VIBE_NONE:
      return "None";
    case TIMER_VIBE_SHORT:
      return shortStr ? "Short" : "Short Pulse";
    case TIMER_VIBE_LONG:
      return shortStr ? "Long" : "Long Pulse";
    case TIMER_VIBE_DOUBLE:
      return shortStr ? "Double" : "Double Pulse";
    case TIMER_VIBE_TRIPLE:
      return shortStr ? "Triple" : "Triple Pulse";
    case TIMER_VIBE_SOLID:
      return shortStr ? "Solid" : "Continous";
  }
  return "";
}

Timer* timer_create_timer(void) {
  Timer* timer = malloc(sizeof(Timer));
  timer->type = TIMER_TYPE_TIMER;
  timer->vibration = settings()->timers_vibration;
  timer->length = settings()->timers_duration;
  timer->wakeup_id = -1;
  timer->timer = NULL;
  timer->repeat = 0;
  timer->label[0] = 0;
  timer->status = TIMER_STATUS_STOPPED;
  timer_set_id(timer);
  return timer;
}

Timer* timer_create_stopwatch(void) {
  Timer* stopwatch = malloc(sizeof(Timer));
  stopwatch->type = TIMER_TYPE_STOPWATCH;
  stopwatch->length = stopwatch->current_time = 0;
  stopwatch->label[0] = 0;
  stopwatch->status = TIMER_STATUS_STOPPED;
  timer_set_id(stopwatch);
  return stopwatch;
}

static void timer_tick(void* context) {
  Timer* timer = (Timer*)context;
  timer->timer = NULL;
  switch (timer->type) {
    case TIMER_TYPE_STOPWATCH:
      timer->current_time += 1;
      break;
    case TIMER_TYPE_TIMER:
      timer->current_time -= 1;
      if (timer->current_time <= 0) {
        timer_finish(timer);
      }
      break;
  }
  if (timer->status == TIMER_STATUS_RUNNING) {
    timer_schedule_tick(timer);
  }
  timers_mark_updated();
}

static void timer_finish(Timer* timer) {
  timer->status = TIMER_STATUS_DONE;
  timer_completed_action(timer);
}

static void timer_schedule_tick(Timer* timer) {
  timer_cancel_tick(timer);
  timer->timer = app_timer_register(1000, timer_tick, (void*)timer);
}

static void timer_cancel_tick(Timer* timer) {
  if (! timer) {
    return;
  }
  if (timer->timer) {
    app_timer_cancel(timer->timer);
    timer->timer = NULL;
  }
}

static void timer_schedule_wakeup(Timer* timer, uint16_t offset) {
  if (! timer) {
    return;
  }
  if (timer->type == TIMER_TYPE_STOPWATCH) {
    return;
  }
  timer_cancel_wakeup(timer);
  time_t wakeup_time = time(NULL);
  wakeup_time += timer->current_time;
  wakeup_time -= 2;
  wakeup_time -= offset;
  timer->wakeup_id = wakeup_schedule(wakeup_time, timer->id, false);
  if (timer->wakeup_id >= 0) {
    return;
  }
  switch (timer->wakeup_id) {
    case E_RANGE: {
      Timer* timer_collision = timers_find_wakeup_collision(timer);
      if (timer_collision) {
        // There is an internal wakeup collision.
        // Should do some magic to make sure the earliest wakeup will fire.
      }
      else {
        // There is an external wakeup.
        // We will do the boring magic to schedule this wakeup early.
      }
      // TODO: Make this smarter!
      timer_schedule_wakeup(timer,  offset - 20);
      break;
    }
    case E_OUT_OF_RESOURCES: {
      // If we've run out of wakeups, then we can just cancel the wake up
      // for whatever the last timer is and schedule this one instead.
      Timer* last_timer = timers_find_last_wakeup();
      if (NULL == last_timer) {
        return;
      }
      if (timer->id == last_timer->id) {
        return;
      }
      else {
        timer_cancel_wakeup(last_timer);
        timer_schedule_wakeup(timer, 0);
      }
      break;
    }
    case E_INVALID_ARGUMENT:
      // This will probably occur when trying to schedule around conflicts.
      break;
  }
}

static void timer_cancel_wakeup(Timer* timer) {
  if (! timer) {
    return;
  }
  if (timer->wakeup_id <= 0) {
    return;
  }
  wakeup_cancel(timer->wakeup_id);
  timer->wakeup_id = -1;
}

static void timer_set_id(Timer* timer) {
  timer->id = rand();
  while (timers_find(timer->id)) {
    timer->id = rand();
  }
}

static void timer_completed_action(Timer* timer) {
  switch (timer->vibration) {
    case TIMER_VIBE_NONE:
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
      break;
    }
    case TIMER_VIBE_TRIPLE: {
      const uint32_t seg[] = { 600, 200, 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
      break;
    }
    case TIMER_VIBE_SOLID:
      win_vibrate_show();
      break;
    default:
      break;
  }
  if (timer->repeat == TIMER_REPEAT_INFINITE) {
    timer_start(timer);
  }
  timers_highlight(timer);
}
