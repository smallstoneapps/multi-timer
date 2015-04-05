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

static void timer_finish(Timer* timer, TimerTimestamp reference);
static void timer_schedule_wakeup(Timer* timer, uint16_t offset);
static void timer_cancel_wakeup(Timer* timer);
static void timer_set_id(Timer* timer);
static void timer_completed_action(Timer* timer);
static void timer_transition_to_stop(Timer* timer, TimerTimestamp reference_time);
static void timer_transition_to_start(Timer* timer, TimerTimestamp reference_time);
static void timer_transition_to(Timer* timer, TimerTimestamp reference_time, TimerStatus new_status);
static void timer_set_running_time(Timer* timer, TimerTimestamp reference_time, int32_t running_time);
static bool timer_has_finished(Timer* timer, TimerTimestamp reference);

TimerTimestamp timer_get_end_timestamp(Timer* timer) {
  switch (timer->type) {
    case TIMER_TYPE_TIMER: return timer->start_timestamp + timer->length - timer->paused_offset;
    default: return 0;
  }
}

uint32_t timer_get_running_time(Timer* timer, TimerTimestamp reference_time) {
  if (TIMER_STATUS_RUNNING == timer->status) {
    return reference_time - timer->start_timestamp + timer->paused_offset;
  } else {
    // Both reference_time and timer->start_timestamp are meaningless.
    return timer->paused_offset;
  }
}

static void timer_transition_to_stop(Timer* timer, TimerTimestamp reference_time) {
  timer->paused_offset += reference_time - timer->start_timestamp;
}

static void timer_transition_to_start(Timer* timer, TimerTimestamp reference_time) {
  timer->start_timestamp = reference_time;
}

static void timer_transition_to(Timer* timer, TimerTimestamp reference_time, TimerStatus new_status) {
  if (TIMER_STATUS_RUNNING != timer->status && TIMER_STATUS_RUNNING == new_status) {
    timer_transition_to_start(timer, reference_time);
  } else if (TIMER_STATUS_RUNNING == timer-> status && TIMER_STATUS_RUNNING != new_status) {
    timer_transition_to_stop(timer, reference_time);
  }
  timer->status = new_status;
}

static void timer_set_running_time(Timer* timer, TimerTimestamp reference_time, int32_t running_time) {
  timer->start_timestamp = reference_time;
  timer->paused_offset = running_time;
}

int32_t timer_get_display_time(Timer* timer, TimerTimestamp reference_time) {
  switch (timer->type) {
    case TIMER_TYPE_TIMER: return timer->length - timer_get_running_time(timer, reference_time);
    case TIMER_TYPE_STOPWATCH: return timer_get_running_time(timer, reference_time);
    default: return 0;
 }
}

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

void timer_display_str(Timer* timer, TimerTimestamp reference, bool showHours, char* str, int str_len) {
  int32_t current_time = timer_get_display_time(timer, reference);
  return timer_time_str(current_time, showHours, str, str_len);
}

void timer_start(Timer* timer, TimerTimestamp reference) {
  timer_reset(timer);
  timer_resume(timer, reference);
}

void timer_pause(Timer* timer, TimerTimestamp reference) {
  timer_transition_to(timer, reference, TIMER_STATUS_PAUSED);
  timer_cancel_wakeup(timer);
  timers_mark_updated();
}

void timer_resume(Timer* timer, TimerTimestamp reference) {
  timer_transition_to(timer, reference, TIMER_STATUS_RUNNING);
  timer_schedule_wakeup(timer, 0);
  timers_mark_updated();
}

void timer_reset(Timer* timer) {
  // Magic starts
  timer_transition_to(timer, 0, TIMER_STATUS_STOPPED);
  timer_set_running_time(timer, 0, 0);
  // The next timer_transition_to(timer, reference, TIMER_STATUS_RUNNING)
  // call on this timer will work correctly. This is because the reference
  // time doesn't matter on a stop -> start transition.
  // Magic ends
  timer_cancel_wakeup(timer);
  timers_mark_updated();
}

void timer_restart(Timer* timer, TimerTimestamp reference) {
  timer_transition_to(timer, reference, TIMER_STATUS_RUNNING);
  uint32_t running_time = timer_get_running_time(timer, reference);
  timer_set_running_time(timer, reference, running_time % timer->length);
  timer_schedule_wakeup(timer, 0);
  timers_mark_updated();
}

void timer_restore(Timer* timer, TimerTimestamp reference) {
  timer_tick(timer, reference);

  if (timer->status == TIMER_STATUS_RUNNING) {
    timer_resume(timer, reference);
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
  timer->paused_offset = 0;
  timer->wakeup_id = -1;
  timer->repeat = 0;
  timer->label[0] = 0;
  timer->status = TIMER_STATUS_STOPPED;
  timer_set_id(timer);
  return timer;
}

Timer* timer_create_stopwatch(void) {
  Timer* stopwatch = malloc(sizeof(Timer));
  stopwatch->type = TIMER_TYPE_STOPWATCH;
  stopwatch->length = 0;
  stopwatch->paused_offset = 0;
  stopwatch->label[0] = 0;
  stopwatch->status = TIMER_STATUS_STOPPED;
  timer_set_id(stopwatch);
  return stopwatch;
}

void timer_tick(Timer* timer, TimerTimestamp reference) {
  if (TIMER_STATUS_RUNNING == timer->status &&
      TIMER_TYPE_TIMER == timer->type &&
      timer_has_finished(timer, reference)) {
    timer_finish(timer, reference);
  }
  if (TIMER_STATUS_RUNNING == timer->status) {
    timers_mark_updated();
  }
}

static bool timer_has_finished(Timer* timer, TimerTimestamp reference) {
  return reference >= timer_get_end_timestamp(timer);
}

static void timer_finish(Timer* timer, TimerTimestamp reference) {
  timer_transition_to(timer, reference, TIMER_STATUS_DONE);
  if (timer->repeat == TIMER_REPEAT_INFINITE) {
    timer_restart(timer, reference);
  } else {
    timer_set_running_time(timer, reference, timer->length);
  }
  timer_completed_action(timer);
}

static void timer_schedule_wakeup(Timer* timer, uint16_t offset) {
  if (! timer) {
    return;
  }
  if (timer->type == TIMER_TYPE_STOPWATCH) {
    return;
  }
  timer_cancel_wakeup(timer);
  TimerTimestamp wakeup_time = timer_get_end_timestamp(timer);
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
  timers_highlight(timer);
}
