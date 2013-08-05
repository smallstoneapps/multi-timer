#ifndef TIMER_H
#define TIMER_H

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
  TIMER_VIBE_OFF = 0,
  TIMER_VIBE_SHORT = 1,
  TIMER_VIBE_LONG = 2,
  TIMER_VIBE_DOUBLE = 3,
  TIMER_VIBE_TRIPLE = 4,
  TIMER_VIBE_CONTINUOUS = 5
} TimerVibration;

typedef struct {
  int length;
  int time_left;
  TimerStatus status;
  TimerDirection direction;
  TimerVibration vibrate;
} Timer;

void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);
void timer_tick(Timer* timer);

#endif