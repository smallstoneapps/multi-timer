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

typedef struct Timer {
  uint16_t id;
  TimerType type;
  uint32_t length;
  uint32_t current_time;
  TimerStatus status;
  TimerVibration vibration;
  uint8_t repeat;
  uint8_t repeat_count;
  AppTimer* timer;
  WakeupId wakeup_id;
  char label[24];
} Timer;

#define TIMER_REPEAT_INFINITE 100

void timer_time_str(uint32_t timer_time, bool showHours, char* str, int str_len);
void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);
void timer_restore(Timer* timer, uint16_t seconds_elapsed);
Timer* timer_clone(Timer* timer);
char* timer_vibe_str(TimerVibration vibe, bool shortStr);
Timer* timer_create_timer(void);
Timer* timer_create_stopwatch(void);
