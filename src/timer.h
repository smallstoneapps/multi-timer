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

src/timer.h

*/

#pragma once

#include <pebble.h>

typedef enum {
  TIMER_TYPE_TIMER = 0,
  TIMER_TYPE_STOPWATCH = 1,
} TimerType;

typedef enum {
  TIMER_VIBE_NONE = 0,
  TIMER_VIBE_SHORT = 1,
  TIMER_VIBE_LONG = 2,
  TIMER_VIBE_DOUBLE = 3,
  TIMER_VIBE_TRIPLE = 4,
  TIMER_VIBE_SOLID = 5
} TimerVibration;

typedef enum {
  TIMER_STATUS_STOPPED = 0,
  TIMER_STATUS_RUNNING = 1,
  TIMER_STATUS_PAUSED = 2,
  TIMER_STATUS_DONE = 3,
} TimerStatus;

typedef time_t TimerTimestamp;

typedef struct Timer {
  uint16_t id;
  TimerType type;
  uint32_t length;
  int32_t paused_offset;
  TimerTimestamp start_timestamp;
  TimerStatus status;
  TimerVibration vibration;
  uint8_t repeat;
  uint8_t repeat_count;
  WakeupId wakeup_id;
  char label[24];
} Timer;

#define TIMER_REPEAT_INFINITE 100

void timer_time_str(uint32_t timer_time, bool showHours, char* str, int str_len);
void timer_display_str(Timer* timer, TimerTimestamp reference, bool showHours, char* str, int str_len);
void timer_start(Timer* timer, TimerTimestamp reference);
void timer_pause(Timer* timer, TimerTimestamp reference);
void timer_resume(Timer* timer, TimerTimestamp reference);
void timer_reset(Timer* timer);
void timer_restart(Timer* timer, TimerTimestamp reference);
void timer_restore(Timer* timer, TimerTimestamp reference);
void timer_restore_legacy(Timer* timer, TimerTimestamp reference, uint32_t offset, uint32_t display_time);
Timer* timer_clone(Timer* timer);
char* timer_vibe_str(TimerVibration vibe, bool shortStr);
Timer* timer_create_timer(void);
Timer* timer_create_stopwatch(void);
void timer_tick(Timer* timer, TimerTimestamp reference);
uint32_t timer_get_running_time(Timer* timer, TimerTimestamp reference);
TimerTimestamp timer_get_end_timestamp(Timer* timer);
int32_t timer_get_display_time(Timer* timer, TimerTimestamp reference);
