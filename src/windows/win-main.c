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

src/windows/win-main.c

*/

#include <pebble.h>

#include <pebble-assist.h>
#include <bitmap-loader.h>

#include "win-about.h"
#include "win-controls.h"
#include "win-timer-add.h"
#include "win-timer.h"
#include "win-settings.h"
#include "win-vibration.h"
#include "win-duration.h"
#include "win-vibrate.h"

#include "../common.h"
#include "../icons.h"
#include "../timers.h"
#include "../settings.h"

#define MENU_SECTION_CLOCK   0
#define MENU_SECTION_TIMERS  1
#define MENU_SECTION_OTHER   2

#define MENU_ROW_COUNT_OTHER 5

#define MENU_ROW_OTHER_ADD_TIMER     0
#define MENU_ROW_OTHER_ADD_STOPWATCH 1
#define MENU_ROW_OTHER_CONTROLS      2
#define MENU_ROW_OTHER_SETTINGS      3
#define MENU_ROW_OTHER_ABOUT         4

static void window_load(Window* window);
static void window_unload(Window* window);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context);
static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context);
static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context);
static void menu_draw_row_clock(GContext* ctx, const Layer* cell_layer);
static void menu_draw_row_timers(GContext* ctx, const Layer* cell_layer, uint16_t row_index);
static void menu_draw_row_other(GContext* ctx, const Layer* cell_layer, uint16_t row_index);
static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context);
static void menu_select_timers(uint16_t row_index);
static void menu_select_other(uint16_t row_index);
static void menu_select_long(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context);
static void timers_update_handler(void);
static void timer_highlight_handler(Timer* timer);

static Window*    s_window;
static MenuLayer* s_menu;

void win_main_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  timers_register_update_handler(timers_update_handler);
  timers_register_highlight_handler(timer_highlight_handler);
  win_timer_add_init();
  win_timer_init();
  win_about_init();
  win_controls_init();
  win_settings_init();
  win_vibration_init();
  win_duration_init();
  win_vibrate_init();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void win_main_show(void) {
  window_stack_push(s_window, false);
}

static void window_load(Window* window) {
  s_menu = menu_layer_create_fullscreen(s_window);
  menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_num_sections,
    .get_num_rows = menu_num_rows,
    .get_cell_height = menu_cell_height,
    .draw_row = menu_draw_row,
    .select_click = menu_select,
    .select_long_click = menu_select_long,
  });
  menu_layer_add_to_window(s_menu, s_window);
}

static void window_unload(Window* window) {
  menu_layer_destroy(s_menu);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (settings()->show_clock) {
    menu_layer_reload_data(s_menu);
  }
}

static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context) {
  return 3;
}

static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context) {
  switch (section_index) {
    case MENU_SECTION_CLOCK:
      return settings()->show_clock ? 1 : 0;
    case MENU_SECTION_TIMERS:
      return timers_count();
    case MENU_SECTION_OTHER:
      return MENU_ROW_COUNT_OTHER;
    default:
      return 0;
  }
}

static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_TIMERS: {
      Timer* timer = timers_get(cell_index->row);
      if (! timer) {
        return 32;
      }
      switch (timer->type) {
        case TIMER_TYPE_TIMER:
          return 34;
        case TIMER_TYPE_STOPWATCH:
          return 32;
      }
      break;
    }
  }
  return 32;
}

static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_CLOCK:
      menu_draw_row_clock(ctx, cell_layer);
      break;
    case MENU_SECTION_TIMERS:
      menu_draw_row_timers(ctx, cell_layer, cell_index->row);
      break;
    case MENU_SECTION_OTHER:
      menu_draw_row_other(ctx, cell_layer, cell_index->row);
      break;
  }
}

static void menu_draw_row_clock(GContext* ctx, const Layer* cell_layer) {
  char time_str[10];
  clock_copy_time_string(time_str, 10);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, time_str,
    fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
    GRect(33, -3, PEBBLE_WIDTH - 33, 28), GTextOverflowModeFill,
    GTextAlignmentLeft, NULL);
  graphics_draw_bitmap_in_rect(ctx,
    bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_CLOCK),
    GRect(8, 8, 16, 16));
}

static void menu_draw_row_timers(GContext* ctx, const Layer* cell_layer, uint16_t row_index) {
  Timer* timer = timers_get(row_index);
  if (! timer) { return; }
  timer_draw_row(timer, ctx);
}

static void menu_draw_row_other(GContext* ctx, const Layer* cell_layer, uint16_t row_index) {
  switch (row_index) {
    case MENU_ROW_OTHER_ADD_TIMER:
      menu_draw_row_icon_text(ctx, "Timer", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_ADD));
      break;
    case MENU_ROW_OTHER_ADD_STOPWATCH:
      menu_draw_row_icon_text(ctx, "Stopwatch", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_ADD));
      break;
    case MENU_ROW_OTHER_CONTROLS:
      menu_draw_row_icon_text(ctx, "Controls", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_CONTROLS));
      break;
    case MENU_ROW_OTHER_SETTINGS:
      menu_draw_row_icon_text(ctx, "Settings", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_SETTINGS));
      break;
    case MENU_ROW_OTHER_ABOUT:
      menu_draw_row_icon_text(ctx, "About", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16,  ICON_RECT_ABOUT));
      break;
  }
}

static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_TIMERS:
      menu_select_timers(cell_index->row);
      break;
    case MENU_SECTION_OTHER:
      menu_select_other(cell_index->row);
      break;
  }
}

static void menu_select_timers(uint16_t row_index) {
  Timer* timer = timers_get(row_index);
  if (! timer) { return; }

  switch (timer->status) {
    case TIMER_STATUS_STOPPED: {
      timer_start(timer);
      break;
    }
    case TIMER_STATUS_RUNNING:
      timer_pause(timer);
      break;
    case TIMER_STATUS_PAUSED:
      timer_resume(timer);
      break;
    case TIMER_STATUS_DONE:
      timer_reset(timer);
      break;
  }
}

static void menu_select_other(uint16_t row_index) {
  switch (row_index) {
    case MENU_ROW_OTHER_ADD_TIMER:
      win_timer_add_show_new();
      break;
    case MENU_ROW_OTHER_ADD_STOPWATCH: {
      Timer* stopwatch = timer_create_stopwatch();
      if (settings()->timers_start_auto) {
        timer_start(stopwatch);
      }
      timers_add(stopwatch);
      timers_mark_updated();
      timers_highlight(stopwatch);
      break;
    }
    case MENU_ROW_OTHER_ABOUT:
      win_about_show();
      break;
    case MENU_ROW_OTHER_CONTROLS:
      win_controls_show();
      break;
    case MENU_ROW_OTHER_SETTINGS:
      win_settings_show();
      break;
  }
}

static void menu_select_long(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context) {
  if (cell_index->section == MENU_SECTION_TIMERS) {
    Timer* timer = timers_get(cell_index->row);
    win_timer_set_timer(timer);
    win_timer_show();
  }
}

static void timers_update_handler(void) {
  menu_layer_reload_data(s_menu);
}

static void timer_highlight_handler(Timer* timer) {
  uint16_t index = timers_index_of(timer->id);
  menu_layer_set_selected_index(s_menu, (MenuIndex) { .section = 1, .row = index }, MenuRowAlignCenter, true);
}