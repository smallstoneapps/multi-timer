/*

Multi Timer v3.0
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

src/timer.c



#include <pebble.h>
#include <pebble-assist.h>
#include <data-processor.h>
#include <bitmap-loader.h>

#include "timer.h"

#include "windows/win-main.h"
#include "windows/win-vibrate.h"
#include "common.h"
#include "settings.h"
#include "timers.h"

static void timer_callback(void* data);
static void timer_set_app_timer(Timer* timer);
static void timer_cancel_app_timer(Timer* timer);
static void timer_finished(Timer* timer);

void timer_init(Timer* timer) {
  if (! timer) { return; }
  timer->status = TIMER_STATUS_STOPPED;
  timer->app_timer = NULL;
  if (TIMER_DIRECTION_UP == timer->direction) {
    timer->length = 0;
    timer->time_left = 0;
  }
  else {
    timer->time_left = timer->length;
  }
}

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

void timer_destroy(Timer* timer) {
  if (! timer) { return; }
  timer_cancel_app_timer(timer);
  free_safe(timer);
}

Timer* timer_clone(Timer* timer) {
  Timer* clone = malloc(sizeof(Timer));
  clone->direction = timer->direction;
  memcpy(clone->label, timer->label, 24);
  clone->time_left = timer->time_left;
  clone->app_timer = timer->app_timer;
  clone->id = timer->id;
  clone->length = timer->length;
  clone->repeat = timer->repeat;
  clone->vibrate = timer->vibrate;
  clone->status = timer->status;
  return clone;
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

// X|XXXXXX|X|X|
char* timer_serialize(Timer* timer, char delim) {
  char* str = malloc(TIMER_STR_LENGTH);
  if (timer->direction == TIMER_DIRECTION_UP) {
    snprintf(str, TIMER_STR_LENGTH, "%d%c%d%c%s", timer->id, delim,
      timer->direction, delim, timer->label);
  }
  else {
    snprintf(str, TIMER_STR_LENGTH, "%d%c%d%c%d%c%d%c%d%c%s", timer->id, delim,
      timer->direction, delim, (int)timer->length, delim, timer->repeat,
      delim, timer->vibrate, delim, timer->label);
  }
  return str;
}

Timer* timer_deserialize(char* data, char delim) {
  ProcessingState* ps = data_processor_create(data, delim);
  Timer* timer = malloc(sizeof(Timer));
  timer->id = data_processor_get_int(ps);
  timer->direction = data_processor_get_int(ps);
  if (timer->direction == TIMER_DIRECTION_DOWN) {
    timer->length = data_processor_get_int(ps);
    timer->repeat = data_processor_get_bool(ps);
    timer->vibrate = data_processor_get_int(ps);
  }
  char* label_tmp = data_processor_get_string(ps);
  strncpy(timer->label, label_tmp, 24);
  free(label_tmp);
  data_processor_destroy(ps);
  return timer;
}


This function is often used when trying to debug strange issues with persistent
storage. It is not needed most of the time, which is why I am keeping it
commented out.

char* timer_describe(Timer* timer) {
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
  snprintf(description, 512, 
    "%d: I am a %s that is %s, length %d, %d left, %s, %s", timer->id, type, 
    status, (int)timer->length, (int)timer->time_left, repeat, vibrate);
  return description;
}



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
    graphics_draw_bitmap_in_rect(ctx, row_bmp, GRect(10, 10, 16, 16));
  }
  graphics_draw_text(ctx, time_left, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(36, -1, 88, 28), 0, GTextAlignmentLeft, NULL);
  if (dir_bmp != NULL) {
    graphics_draw_bitmap_in_rect(ctx, dir_bmp, GRect(128, 10, 8, 16));
  }

  if (strlen(timer->label) > 0) {
    graphics_draw_text(ctx, timer->label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(10, 24, 120, 18), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  }

  free_safe(time_left);
}

void timer_duration_str(int duration, bool showHours, char* str, int str_len) {
  int hours = duration / 3600;
  int minutes = (showHours ? (duration % 3600) : duration) / 60;
  int seconds = (showHours ? (duration % 3600) : duration) % 60;
  if (showHours) {
    snprintf(str, str_len, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  else {
    snprintf(str, str_len, "%02d:%02d", minutes, seconds);
  }
}

Timer* timer_create(void) {
  Timer* timer = malloc(sizeof(Timer));
  timer->direction = TIMER_DIRECTION_DOWN;
  timer->vibrate = settings()->timers_vibration;
  timer->length = 10 * 60;
  timer->repeat = false;
  timer->label[0] = 0;
  timer->id = rand() % UINT16_MAX;
  while (NULL != timers_find(timer->id)) {
    timer->id = rand() % UINT16_MAX;
  }
  return timer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void timer_callback(void* data) {
  if (NULL == data) {
    return;
  }

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
  win_main_update();
}

static void timer_set_app_timer(Timer* timer) {
  timer_cancel_app_timer(timer);
  timer->app_timer = app_timer_register(1000, timer_callback, (void*)timer);
}

static void timer_cancel_app_timer(Timer* timer) {
  app_timer_cancel_safe(timer->app_timer);
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


*/