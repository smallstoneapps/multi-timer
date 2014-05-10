/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * timer.c
 ***/

#include <pebble.h>
#include "timer.h"
#include "windows/win-timers.h"
#include "windows/win-vibrate.h"
#include "libs/pebble-assist/pebble-assist.h"
#include "libs/data-processor/data-processor.h"
#include "libs/bitmap-loader/bitmap-loader.h"
#include "common.h"
#include "settings.h"

static void timer_callback(void* data);
static void timer_set_app_timer(Timer* timer);
static void timer_cancel_app_timer(Timer* timer);
static void timer_finished(Timer* timer);

void timer_start(Timer* timer) {
  if (! timer) { return; }
  timer->time_left = timer->length;
  timer->status = TIMER_STATUS_RUNNING;
  timer_set_app_timer(timer);
}

void timer_pause(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_PAUSED;
  timer_cancel_app_timer(timer);
}

void timer_resume(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_RUNNING;
  timer_set_app_timer(timer);
}

void timer_reset(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_STOPPED;
  timer->time_left = timer->length;
  timer_cancel_app_timer(timer);
}

void timer_tick(Timer* timer) {
  if (! timer) { return; }
  if (TIMER_STATUS_RUNNING == timer->status) { return; }
  if (TIMER_DIRECTION_DOWN == timer->direction) {
    timer->time_left -= 1;
    if (timer->time_left <= 0) {
      timer_finished(timer);
    }
  }
  else if (TIMER_DIRECTION_UP == timer->direction) {
    timer->time_left += 1;
  }
}

void timer_destroy(Timer* timer) {
  if (! timer) { return; }
  timer_cancel_app_timer(timer);
  free(timer);
}

char* timer_vibe_str(TimerVibration vibe, bool shortStr) {
  switch (vibe) {
    case TIMER_VIBRATION_OFF:
      return "Off";
    case TIMER_VIBRATION_SHORT:
      return shortStr ? "Short" : "Short Pulse";
    case TIMER_VIBRATION_LONG:
      return shortStr ? "Long" : "Long Pulse";
    case TIMER_VIBRATION_DOUBLE:
      return shortStr ? "Double" : "Double Pulse";
    case TIMER_VIBRATION_TRIPLE:
      return shortStr ? "Triple" : "Triple Pulse";
    case TIMER_VIBRATION_CONTINUOUS:
      return shortStr ? "Solid" : "Continous";
  }
  return "";
}

/*char* timer_describe(Timer* timer) {
  static char description[512];
  char type[16];
  char status[16];
  char repeat[16];
  char vibrate[16];

  switch (timer->direction) {
    case TIMER_DIRECTION_DOWN:
      strcpy(type, "countdown");
    break;
    case TIMER_DIRECTION_UP:
      strcpy(type, "stopwatch");
    break;
  }

  switch (timer->status) {
    case TIMER_STATUS_PAUSED:
      strcpy(status, "paused");
    break;
    case TIMER_STATUS_STOPPED:
      strcpy(status, "stopped");
    break;
    case TIMER_STATUS_RUNNING:
      strcpy(status, "running");
    break;
    case TIMER_STATUS_FINISHED:
      strcpy(status, "finished");
    break;
  }

  strcpy(repeat, timer->repeat ? "repeating" : "not repeating");

  strcpy(vibrate, timer_vibe_str(timer->vibrate, true));

  snprintf(description, 512, "I am a %s that is %s, length %d, %d left, %s, %s", type, status, timer->length, timer->time_left, repeat, vibrate);

  return description;
}*/

void timer_draw(Timer* timer, GContext* ctx) {
  char* time_left = malloc(32);
  GBitmap* row_bmp = NULL;
  GBitmap* dir_bmp = NULL;

  if (NULL == timer) {
    return;
  }
  timer_duration_str(timer->time_left, settings()->timers_hours, time_left, 32);

  switch (timer->status) {
    case TIMER_STATUS_RUNNING:
      row_bmp = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY);
    break;
    case TIMER_STATUS_PAUSED:
      row_bmp = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE);
    break;
    case TIMER_STATUS_STOPPED:
      row_bmp = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_STOP);
    break;
    case TIMER_STATUS_FINISHED:
      row_bmp = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_DONE);
    break;
  }

  switch (timer->direction) {
    case TIMER_DIRECTION_UP:
      dir_bmp = bitmaps_get_bitmap(RESOURCE_ID_ARROW_UP);
    break;
    case TIMER_DIRECTION_DOWN:
      dir_bmp = bitmaps_get_bitmap(RESOURCE_ID_ARROW_DOWN);
    break;
  }

  graphics_context_set_text_color(ctx, GColorBlack);
  if (row_bmp != NULL) {
    graphics_draw_bitmap_in_rect(ctx, row_bmp, GRect(8, 10, 20, 20));
  }
  graphics_draw_text(ctx, time_left, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(36, 1, 110, 32), 0, GTextAlignmentLeft, NULL);
  if (dir_bmp != NULL) {
    graphics_draw_bitmap_in_rect(ctx, dir_bmp, GRect(132, 16, 8, 8));
  }

  free(time_left);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void timer_callback(void* data) {
  Timer* timer = (Timer*)data;
  timer->app_timer = NULL;

  if (timer->status != TIMER_STATUS_RUNNING) {
    return;
  }

  switch (timer->direction) {
    case TIMER_DIRECTION_UP:
      timer->time_left += 1;
    break;
    case TIMER_DIRECTION_DOWN:
      timer->time_left -= 1;
      if (timer->time_left == 0) {
        timer_finished(timer);
      }
    break;
  }

  timer_set_app_timer(timer);
  win_timers_update();
}

static void timer_set_app_timer(Timer* timer) {
  timer_cancel_app_timer(timer);
  timer->app_timer = app_timer_register(1000, timer_callback, (void*)timer);
}

static void timer_cancel_app_timer(Timer* timer) {
  if (timer->app_timer != NULL) {
    app_timer_cancel(timer->app_timer);
    timer->app_timer = NULL;
  }
}

static void timer_finished(Timer* timer) {
  timer->status = TIMER_STATUS_FINISHED;
  switch (timer->vibrate) {
    case TIMER_VIBRATION_OFF:
      // Do nothing!
    break;
    case TIMER_VIBRATION_SHORT:
      vibes_short_pulse();
    break;
    case TIMER_VIBRATION_LONG:
      vibes_long_pulse();
    break;
    case TIMER_VIBRATION_DOUBLE: {
      const uint32_t seg[] = { 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBRATION_TRIPLE: {
      const uint32_t seg[] = { 600, 200, 600, 200, 600 };
      VibePattern pattern = {
        .durations =  seg,
        .num_segments = ARRAY_LENGTH(seg)
      };
      vibes_enqueue_custom_pattern(pattern);
    }
    break;
    case TIMER_VIBRATION_CONTINUOUS:
      if (! win_vibrate_is_visible()) {
        win_vibrate_show();
      }
    break;
  }
  if (timer->repeat) {
    timer_start(timer);
  }
}
