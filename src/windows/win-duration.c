/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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

src/windows/win-duration.c

*/

#include <pebble.h>

#include <bitmap-loader.h>
#include <pebble-assist.h>

#include "win-duration.h"

#include "../timer.h"
#include "../common.h"
#include "../icons.h"
#include "../settings.h"

#define MODE_HOURS 0
#define MODE_MINUTES 1
#define MODE_SECONDS 2

static void window_load(Window* window);
static void window_unload(Window* window);
static void layer_update(Layer* me, GContext* ctx);
static void layer_action_bar_click_config_provider(void *context);
static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context);
static void update_timer_length(void);

static Window* s_window;
static Layer* s_layer;
static ActionBarLayer* s_action_bar;
static uint32_t s_duration = 0;
static DurationCallback s_callback;

static int s_mode = MODE_MINUTES;
static int s_hours = 0;
static int s_minutes = 0;
static int s_seconds = 0;

static GFont s_font_duration;

void win_duration_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void win_duration_show(uint16_t duration, DurationCallback callback) {
  s_duration = duration;
  s_callback = callback;
  window_stack_push(s_window, true);
  s_mode = settings()->timers_hours ? MODE_HOURS : MODE_MINUTES;
  s_hours = s_duration / 3600;
  s_minutes = settings()->timers_hours ? ((s_duration % 3600) / 60) : (s_duration / 60);
  s_seconds = settings()->timers_hours ? ((s_duration % 3600) % 60) : (s_duration % 60);
  layer_mark_dirty(s_layer);
}

static void window_load(Window* window) {
  s_layer = layer_create_fullscreen(s_window);
  layer_set_update_proc(s_layer, layer_update);
  layer_add_to_window(s_layer, s_window);

  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, s_window);
  action_bar_layer_set_click_config_provider(s_action_bar, layer_action_bar_click_config_provider);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_ACTION_INC));
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_ACTION_DEC));
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_ACTION_TICK));

  s_font_duration = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AUDI_70_BOLD));
}

static void window_unload(Window* window) {
  action_bar_layer_destroy(s_action_bar);
  layer_destroy(s_layer);
  fonts_unload_custom_font(s_font_duration);
}

static void layer_update(Layer* me, GContext* ctx) {

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  char summary_str[32];
  timer_time_str(s_duration, settings()->timers_hours, summary_str, 32);
  graphics_draw_text(ctx, summary_str, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(0, 0, 144 - 16, 28), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

  // Draw Minutes
  char* time_str = "000";
  char mode_str[16];
  switch (s_mode) {
    case MODE_HOURS:
      snprintf(time_str, 3, "%02d", s_hours);
      snprintf(mode_str, 16, s_hours == 1 ? "HOUR" : "HOURS");
    break;
    case MODE_MINUTES:
      snprintf(time_str, 3, "%02d", s_minutes);
      snprintf(mode_str, 16, s_seconds == 1 ? "MINUTE" : "MINUTES");
    break;
    case MODE_SECONDS:
      snprintf(time_str, 3, "%02d", s_seconds);
      snprintf(mode_str, 16, s_seconds == 1 ? "SECOND" : "SECONDS");
    break;
  }
  graphics_draw_text(ctx, time_str, s_font_duration, GRect(0, 27, 144 - 16, 70), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, mode_str, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 98, 144 - 16, 18), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void layer_action_bar_click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, action_bar_layer_down_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, action_bar_layer_up_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, action_bar_layer_select_handler);
}

static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_mode) {
    case MODE_HOURS:
      s_hours -= 1;
      if (s_hours < 0) {
        s_hours = 0;
      }
    break;
    case MODE_MINUTES:
      s_minutes -= 1;
      if (s_minutes < 0) {
        s_minutes += 60;
      }
    break;
    case MODE_SECONDS:
      s_seconds -= 1;
      if (s_seconds < 0) {
        s_seconds += 60;
      }
    break;
  }
  update_timer_length();
  layer_mark_dirty(s_layer);
}

static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_mode) {
    case MODE_HOURS:
      s_hours += 1;
    break;
    case MODE_MINUTES:
      s_minutes += 1;
      if (s_minutes >= 60 && settings()->timers_hours) {
        s_minutes -= 60;
      }
      if (s_minutes >= 100) {
        s_minutes = 99;
      }
    break;
    case MODE_SECONDS:
      s_seconds += 1;
      if (s_seconds >= 60) {
        s_seconds -= 60;
      }
    break;
  }
  update_timer_length();
  layer_mark_dirty(s_layer);
}

static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_mode) {
    case MODE_HOURS:
      s_mode = MODE_MINUTES;
    break;
    case MODE_MINUTES:
      s_mode = MODE_SECONDS;
    break;
    case MODE_SECONDS:
      s_callback(s_duration);
      window_stack_pop(true);
    break;
  }
  layer_mark_dirty(s_layer);
}

static void update_timer_length(void) {
  if (settings()->timers_hours) {
    s_duration = s_hours * 3600 + s_minutes * 60 + s_seconds;
  }
  else {
    s_duration = s_minutes * 60 + s_seconds;
  }
}
