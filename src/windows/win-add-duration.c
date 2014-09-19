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

src/windows/win-add-duration.c

*/

#include <pebble.h>

#include "win-add-duration.h"
#include <bitmap-loader.h>
#include <pebble-assist.h>
#include "../timer.h"
#include "../common.h"
#include "../settings.h"

#define MODE_HOURS 0
#define MODE_MINUTES 1
#define MODE_SECONDS 2

static void layer_update(Layer* me, GContext* ctx);
static void layer_action_bar_click_config_provider(void *context);
static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context);
static void update_timer_length(void);

static Window* window;
static Layer* layer;
static ActionBarLayer* layer_action_bar;
static Timer* timer = NULL;
static int mode = MODE_MINUTES;
static int hours = 0;
static int minutes = 0;
static int seconds = 0;
static GFont font_duration;

void win_add_duration_init(void) {
  window = window_create();

  layer = layer_create_fullscreen(window);
  layer_set_update_proc(layer, layer_update);
  layer_add_to_window(layer, window);

  layer_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(layer_action_bar, window);
  action_bar_layer_set_click_config_provider(layer_action_bar, layer_action_bar_click_config_provider);
  action_bar_layer_set_icon(layer_action_bar, BUTTON_ID_UP, bitmaps_get_bitmap(RESOURCE_ID_ACTION_INC));
  action_bar_layer_set_icon(layer_action_bar, BUTTON_ID_DOWN, bitmaps_get_bitmap(RESOURCE_ID_ACTION_DEC));
  action_bar_layer_set_icon(layer_action_bar, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ACTION_OK));

  font_duration = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AUDI_70_BOLD));
}

void win_add_duration_show(Timer* tmr) {
  timer = tmr;
  window_stack_push(window, ANIMATE_WINDOWS);
  mode = settings()->timers_hours ? MODE_HOURS : MODE_MINUTES;
  hours = timer->length / 3600;
  minutes = settings()->timers_hours ? ((timer->length % 3600) / 60) : (timer->length / 60);
  seconds = settings()->timers_hours ? ((timer->length % 3600) % 60) : (timer->length % 60);
  layer_mark_dirty(layer);
}

void win_add_duration_destroy(void) {
  action_bar_layer_destroy(layer_action_bar);
  layer_destroy(layer);
  window_destroy(window);
  fonts_unload_custom_font(font_duration);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void layer_update(Layer* me, GContext* ctx) {
  if (timer == NULL) {
    return;
  }

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  char summary_str[32];
  timer_duration_str(timer->length, settings()->timers_hours, summary_str, 32);
  graphics_draw_text(ctx, summary_str, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(0, 0, 144 - 16, 28), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

  // Draw Minutes
  char* time_str = "000";
  char mode_str[16];
  switch (mode) {
    case MODE_HOURS:
      snprintf(time_str, 3, "%02d", hours);
      snprintf(mode_str, 16, hours == 1 ? "HOUR" : "HOURS");
    break;
    case MODE_MINUTES:
      snprintf(time_str, 3, "%02d", minutes);
      snprintf(mode_str, 16, seconds == 1 ? "MINUTE" : "MINUTES");
    break;
    case MODE_SECONDS:
      snprintf(time_str, 3, "%02d", seconds);
      snprintf(mode_str, 16, seconds == 1 ? "SECOND" : "SECONDS");
    break;
  }
  graphics_draw_text(ctx, time_str, font_duration, GRect(0, 27, 144 - 16, 70), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, mode_str, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 98, 144 - 16, 18), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void layer_action_bar_click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, action_bar_layer_down_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, action_bar_layer_up_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, action_bar_layer_select_handler);
}

static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_HOURS:
      hours -= 1;
      if (hours < 0) {
        hours = 0;
      }
    break;
    case MODE_MINUTES:
      minutes -= 1;
      if (minutes < 0) {
        minutes += 60;
      }
    break;
    case MODE_SECONDS:
      seconds -= 1;
      if (seconds < 0) {
        seconds += 60;
      }
    break;
  }
  update_timer_length();
  layer_mark_dirty(layer);
}

static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_HOURS:
      hours += 1;
    break;
    case MODE_MINUTES:
      minutes += 1;
      if (minutes >= 60 && settings()->timers_hours) {
        minutes -= 60;
      }
    break;
    case MODE_SECONDS:
      seconds += 1;
      if (seconds >= 60) {
        seconds -= 60;
      }
    break;
  }
  update_timer_length();
  layer_mark_dirty(layer);
}

static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_HOURS:
      mode = MODE_MINUTES;
    break;
    case MODE_MINUTES:
      mode = MODE_SECONDS;
    break;
    case MODE_SECONDS:
      window_stack_pop(true);
    break;
  }
  layer_mark_dirty(layer);
}

static void update_timer_length(void) {
  if (settings()->timers_hours) {
    timer->length = hours * 3600 + minutes * 60 + seconds;
  }
  else {
    timer->length = minutes * 60 + seconds;
  }
}