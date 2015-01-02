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

src/migration.h

*/

#pragma once

#include <pebble.h>
#include "timers.h"

typedef enum {
  OLD_TIMER_STATUS_STOPPED,
  OLD_TIMER_STATUS_RUNNING,
  OLD_TIMER_STATUS_PAUSED,
  OLD_TIMER_STATUS_FINISHED
} OldTimerStatus;

typedef enum {
  OLD_TIMER_DIRECTION_UP,
  OLD_TIMER_DIRECTION_DOWN
} OldTimerDirection;

typedef enum {
  OLD_TIMER_VIBRATION_OFF,
  OLD_TIMER_VIBRATION_SHORT,
  OLD_TIMER_VIBRATION_LONG,
  OLD_TIMER_VIBRATION_DOUBLE,
  OLD_TIMER_VIBRATION_TRIPLE,
  OLD_TIMER_VIBRATION_CONTINUOUS
} OldTimerVibration;

typedef struct {
  uint16_t id;
  OldTimerDirection direction;
  uint32_t length;
  uint32_t time_left;
  OldTimerStatus status;
  OldTimerVibration vibrate;
  bool repeat;
  AppTimer* app_timer;
  char label[24];
} __attribute__((__packed__)) OldTimer;

typedef struct {
  OldTimer timers[5];
  uint8_t count;
  int time;
} OldTimerBlock;

typedef struct TimerTiny {
  uint16_t id;
  TimerType type;
  uint16_t length;
  uint16_t current_time;
  TimerStatus status;
  TimerVibration vibration;
  uint8_t repeat;
  uint8_t repeat_count;
  AppTimer* timer;
  WakeupId wakeup_id;
  char label[24];
} TimerTiny;

typedef struct {
  TimerTiny timers[TIMER_BLOCK_SIZE];
  uint8_t total_timers;
  time_t save_time;
} TimerBlockTiny;

// Struct of the original settings.
typedef struct {
  bool save_timers_auto; // Automatically save timers on exit and load them when the app restarts?
  bool resume_timers; // Automatically resume running timers when app starts?
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  bool timers_hours; // Use hours in timers?
} OldSettings;

// Struct of the fucked up settings where the timers_duration field was too small
typedef struct {
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  uint16_t timers_duration; // Default timer duration
  bool timers_hours; // Use hours in timers?
  bool show_clock; // Display a clock row in the main screen?
} SettingsTiny;
