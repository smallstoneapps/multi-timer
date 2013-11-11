/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timer.h
 ***/

#pragma once

#include <pebble.h>

typedef enum {
  TIMER_STOPPED,
  TIMER_RUNNING,
  TIMER_PAUSED,
  TIMER_FINISHED
} TimerStatus;

typedef enum {
  TIMER_UP = 0,
  TIMER_DOWN = 1
} TimerDirection;

#define TIMER_VIBE_COUNT 6

typedef enum {
  TIMER_VIBE_OFF,
  TIMER_VIBE_SHORT,
  TIMER_VIBE_LONG,
  TIMER_VIBE_DOUBLE,
  TIMER_VIBE_TRIPLE,
  TIMER_VIBE_CONTINUOUS
} TimerVibration;

typedef struct {
  uint16_t length;
  uint16_t time_left;
  TimerStatus status;
  TimerVibration vibrate;
  bool repeat;
  AppTimer* app_timer;
  TimerDirection direction;
} Timer;

void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);

char* timer_vibe_str(TimerVibration vibe, bool shortStr);
char* timer_stringify(Timer* timer);
Timer* timer_unstringify(char* str);
