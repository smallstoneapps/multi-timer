#include <pebble.h>

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
  bool timers_start_auto; // Should timers start immediately when you add them?
  TimerVibration timers_vibration; // Default timer vibration pattern
  uint16_t timers_duration; // Default timer duration
  bool timers_hours; // Use hours in timers?
  bool show_clock; // Display a clock row in the main screen?
} SettingsTiny;
