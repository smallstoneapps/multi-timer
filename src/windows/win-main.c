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

src/windows/win-main.c

*/

#include <pebble.h>

#include <pebble-assist.h>
#include <bitmap-loader.h>
#include "../timers.h"
#include "../settings.h"
#include "../timer.h"
#include "../common.h"
#include "../analytics.h"
#include "win-add.h"
#include "win-settings.h"
#include "win-vibrate.h"
#include "win-about.h"
#include "win-timer.h"

#define MENU_SECTIONS 4
#define MENU_SECTION_CLOCK 0
#define MENU_SECTION_TIMERS 1
#define MENU_SECTION_ACTIONS 2
#define MENU_SECTION_CONTROLS 3

#define MENU_ROWS_ACTIONS 4
#define MENU_ROWS_CONTROLS 4

#define MENU_ITEM_ACTION_ADD 0
#define MENU_ITEM_ACTION_CONTROLS 1
#define MENU_ITEM_ACTION_SETTINGS 2
#define MENU_ITEM_ACTION_ABOUT 3

#define MENU_ITEM_CONTROL_START 0
#define MENU_ITEM_CONTROL_PAUSE 1
#define MENU_ITEM_CONTROL_RESET 2
#define MENU_ITEM_CONTROL_CLEAR 3

#define ROW_HEIGHT_MAIN 34
#define ROW_HEIGHT_TIMER 36
#define ROW_HEIGHT_LABEL 50

static void window_load(Window* window);
static void window_unload(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_action_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_clock_row(GContext* ctx);
static void menu_draw_control_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_select_control_row(uint16_t row);
static void menu_select_action_row(uint16_t row);
static void menu_select_timer_row(uint16_t row);
static void jump_to_timer(int t, bool animate);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static Window* window;
static MenuLayer* layer_menu;

void win_main_init(void) {
  // Create the window and setup event handlers.
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
  // Initialise all of the windows that can be spawned from this one.
  win_add_init();
  win_settings_init();
  win_vibrate_init();
  win_timer_init();
  win_about_init();
}

void win_main_show(void) {
  window_stack_push(window, ANIMATE_WINDOWS);
}

void win_main_update(void) {
  menu_layer_reload_data(layer_menu);
}

void win_main_destroy(void) {
  win_vibrate_destroy();
  win_add_destroy();
  win_settings_destroy();
  win_about_destroy();
  win_timer_destroy();
  window_destroy(window);
}

void win_main_jump(int pos) {
  jump_to_timer(pos, false);
}

//----------------------------------------------------------------------------//

static void window_load(Window* window) {
  // Create the menu with callbacks and click config.
  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
     .draw_header = menu_draw_header_callback,

    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
    .select_long_click = menu_select_long_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void window_unload(Window* window) {
  menu_layer_destroy(layer_menu);
}

static void window_appear(Window* window) {
  menu_layer_reload_data(layer_menu);
}

// This function exists because it actually reduces the code size to have all
// four window handlers implemented instead of just three.
static void window_disappear(Window* window) {

}

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_CLOCK:
      return settings()->show_clock ? 1 : 0;
    case MENU_SECTION_TIMERS:
      return timers_get_count();
    case MENU_SECTION_ACTIONS:
      return MENU_ROWS_ACTIONS;
    case MENU_SECTION_CONTROLS:
      return MENU_ROWS_CONTROLS;
  }
  return 0;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_ACTIONS:
      if (timers_get_count() == 0) {
        return 0;
      }
      return 4;
    case MENU_SECTION_CONTROLS:
      return 4;
    case MENU_SECTION_TIMERS:
      return 0;
  }
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  switch (cell_index->section) {
    case MENU_SECTION_TIMERS: {
      Timer* timer = timers_get(cell_index->row);
      return strlen(timer->label) > 0 ? ROW_HEIGHT_LABEL : ROW_HEIGHT_TIMER;
    }
    break;
    case MENU_SECTION_ACTIONS:
    case MENU_SECTION_CONTROLS:
    case MENU_SECTION_CLOCK:
      return ROW_HEIGHT_MAIN;
    break;
  }
  return 0;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {

}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case MENU_SECTION_CLOCK:
      menu_draw_clock_row(ctx);
      break;
    case MENU_SECTION_TIMERS:
      menu_draw_timer_row(ctx, cell_layer, cell_index, data);
      break;
    case MENU_SECTION_ACTIONS:
      menu_draw_action_row(ctx, cell_layer, cell_index, data);
      break;
    case MENU_SECTION_CONTROLS:
      menu_draw_control_row(ctx, cell_layer, cell_index, data);
      break;
  }
}

static void menu_draw_action_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENU_ITEM_ACTION_ADD:
      draw_icon_text_row(ctx, "Add Timer", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_ADD));
      break;
    case MENU_ITEM_ACTION_CONTROLS:
      draw_icon_text_row(ctx, "Controls", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CONTROLS));
      break;
    case MENU_ITEM_ACTION_SETTINGS:
      draw_icon_text_row(ctx, "Settings", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_SETTINGS));
      break;
    case MENU_ITEM_ACTION_ABOUT:
      draw_icon_text_row(ctx, "About", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_ABOUT));
      break;
  }
}

static void menu_draw_control_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENU_ITEM_CONTROL_START:
      draw_icon_text_row(ctx, "Start All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY));
      break;
    case MENU_ITEM_CONTROL_PAUSE:
      draw_icon_text_row(ctx, "Pause All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE));
      break;
    case MENU_ITEM_CONTROL_RESET:
      draw_icon_text_row(ctx, "Reset All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_RESET));
      break;
    case MENU_ITEM_CONTROL_CLEAR:
      draw_icon_text_row(ctx, "Clear Timers", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLEAR));
      break;
  }
}

static void menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  Timer* timer = timers_get(cell_index->row);
  timer_draw(timer, ctx);
}

static void menu_draw_clock_row(GContext* ctx) {
  char time_str[16];
  time_t the_time = time(NULL);
  struct tm* now = localtime(&the_time);
  strftime(time_str, 16, "%H:%M", now);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, time_str, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(36, -1, 88, 28), 0, GTextAlignmentLeft, NULL);
  graphics_draw_bitmap_in_rect(ctx, bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLOCK), GRect(10, 10, 16, 16));
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_ACTIONS:
      menu_select_action_row(cell_index->row);
      break;
    case MENU_SECTION_CONTROLS:
      menu_select_control_row(cell_index->row);
      break;
    case MENU_SECTION_TIMERS:
      menu_select_timer_row(cell_index->row);
      break;
  }
  menu_layer_reload_data(layer_menu);
}

static void menu_select_action_row(uint16_t row) {
  switch (row) {
    case MENU_ITEM_ACTION_ADD:
      analytics_track_event("menu_click", "item`add_timer");
      win_add_show_new();
      break;
    case MENU_ITEM_ACTION_CONTROLS:
      analytics_track_event("menu_click", "item`controls");
      menu_layer_set_selected_index(layer_menu, (MenuIndex) {
        .section = MENU_SECTION_CONTROLS,
        .row = 0
      }, MenuRowAlignCenter, true);
      break;
    case MENU_ITEM_ACTION_SETTINGS:
      analytics_track_event("menu_click", "item`settings");
      win_settings_show();
      break;
    case MENU_ITEM_ACTION_ABOUT:
      analytics_track_event("menu_click", "item`about");
      win_about_show();
      break;
  }
}

static void menu_select_control_row(uint16_t row) {
  switch (row) {
    case MENU_ITEM_CONTROL_START:
      analytics_track_event("menu_click", "item`start_all");
      for (int t = 0; t < timers_get_count(); t += 1) {
        if (timers_get(t)->status == TIMER_STATUS_PAUSED) {
          timer_resume(timers_get(t));
        }
        else if (timers_get(t)->status == TIMER_STATUS_STOPPED) {
          timer_start(timers_get(t));
        }
      }
      win_main_jump(0);
    break;
    case MENU_ITEM_CONTROL_PAUSE:
      analytics_track_event("menu_click", "item`pause_all");
      for (int t = 0; t < timers_get_count(); t += 1) {
        timer_pause(timers_get(t));
      }
      win_main_jump(0);
    break;
    case MENU_ITEM_CONTROL_RESET:
      analytics_track_event("menu_click", "item`reset_all");
      for (int t = 0; t < timers_get_count(); t += 1) {
        timer_reset(timers_get(t));
      }
      win_main_jump(0);
    break;
    case MENU_ITEM_CONTROL_CLEAR:
      analytics_track_event("menu_click", "item`clear_all");
      timers_clear();
      timers_send_list();
    break;
  }
  menu_layer_set_selected_index(layer_menu,
    (MenuIndex) {
      .section = (timers_get_count() == 0) ? MENU_SECTION_ACTIONS : MENU_SECTION_TIMERS,
      .row = 0
    }, MenuRowAlignCenter, true);
}

static void menu_select_timer_row(uint16_t row) {
  if (timers_get_count() == 0) {
    return;
  }
  Timer* timer = timers_get(row);
  if (! timer) {
    return;
  }
  switch (timer->status) {
    case TIMER_STATUS_RUNNING:
      timer_pause(timer);
      break;
    case TIMER_STATUS_PAUSED:
      timer_resume(timer);
      break;
    case TIMER_STATUS_STOPPED:
      timer_start(timer);
      break;
    case TIMER_STATUS_FINISHED:
      timer_reset(timer);
      break;
  }
}

static void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if (cell_index->section != MENU_SECTION_TIMERS) {
    return;
  }
  win_timer_show(timers_get(cell_index->row), cell_index->row);
}

static void jump_to_timer(int t, bool animate) {
  MenuIndex index =  { MENU_SECTION_TIMERS, t };
  menu_layer_set_selected_index(layer_menu, index, MenuRowAlignCenter, animate);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  menu_layer_reload_data(layer_menu);
}