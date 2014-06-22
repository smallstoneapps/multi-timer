/*

Multi Timer v2.7.0
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

src/timer.h

*/

#pragma once

#include <pebble.h>

#define TIMER_STR_LENGTH 52

typedef enum {
  TIMER_STATUS_STOPPED,
  TIMER_STATUS_RUNNING,
  TIMER_STATUS_PAUSED,
  TIMER_STATUS_FINISHED
} TimerStatus;

typedef enum {
  TIMER_DIRECTION_UP,
  TIMER_DIRECTION_DOWN
} TimerDirection;

#define TIMER_VIBRATION_COUNT 6

typedef enum {
  TIMER_VIBRATION_OFF,
  TIMER_VIBRATION_SHORT,
  TIMER_VIBRATION_LONG,
  TIMER_VIBRATION_DOUBLE,
  TIMER_VIBRATION_TRIPLE,
  TIMER_VIBRATION_CONTINUOUS
} TimerVibration;

typedef struct Timer {
  uint16_t id;
  TimerDirection direction;
  uint32_t length;
  uint32_t time_left;
  TimerStatus status;
  TimerVibration vibrate;
  bool repeat;
  AppTimer* app_timer;
  char label[24];
} __attribute__((__packed__)) Timer;

void timer_init(Timer* timer);
void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);
void timer_destroy(Timer* timer);
void timer_set_label(Timer* timer);
Timer* timer_clone(Timer* timer);

char* timer_vibe_str(TimerVibration vibe, bool shortStr);
char* timer_serialize(Timer* timer, char delim);
void timer_draw(Timer* timer, GContext* ctx);
char* timer_describe(Timer* timer);
void timer_duration_str(int duration, bool showHours, char* str, int str_len);