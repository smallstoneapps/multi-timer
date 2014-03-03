/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timer.h
 ***/

#pragma once

#include <pebble.h>

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

typedef struct {
  TimerDirection direction;
  int length;
  int time_left;
  TimerStatus status;
  TimerVibration vibrate;
  bool repeat;
  AppTimer* app_timer;
} __attribute__((__packed__)) Timer;

void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);

char* timer_vibe_str(TimerVibration vibe, bool shortStr);
char* timer_stringify(Timer* timer);
Timer* timer_unstringify(char* str);
char* timer_describe(Timer* timer);
