#ifndef TIMER_H
#define TIMER_H

typedef enum {
  TIMER_STOPPED = 0,
  TIMER_RUNNING = 1,
  TIMER_PAUSED = 2,
  TIMER_FINISHED = 3
} TimerStatus;

typedef struct {
  int length;
  int time_left;
  TimerStatus status;
} Timer;

void timer_start(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
void timer_reset(Timer* timer);
void timer_tick(Timer* timer);

#endif