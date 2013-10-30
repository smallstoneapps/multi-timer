/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * timer.h
 ***/

#pragma once

#include <pebble_os.h>

typedef enum {
  TIMER_STOPPED = 0,
  TIMER_RUNNING = 1,
  TIMER_PAUSED = 2,
  TIMER_FINISHED = 3
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
  int length;
  int time_left;
  TimerStatus status;
  TimerDirection direction;
  TimerVibration vibrate;
  bool repeat;
  AppTimer* app_timer;
} Timer;

void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);
