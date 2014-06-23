/*

Multi Timer v2.8.0

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

src/windows/win-settings.c

*/

#include <pebble.h>

#include "../libs/pebble-assist/pebble-assist.h"
#include "win-settings.h"
#include "win-vibration.h"
#include "../settings.h"
#include "../timers.h"
#include "../common.h"

#define MENU_NUM_SECTIONS 2
#define MENU_SECTION_SAVE 0
#define MENU_SECTION_TIMERS 1

#define MENU_SECTION_ROWS_SAVE 2
#define MENU_SECTION_ROWS_TIMERS 3

#define MENU_ROW_SAVE_AUTO 0
#define MENU_ROW_SAVE_RESUME 1
#define MENU_ROW_TIMERS_START 0
#define MENU_ROW_TIMERS_VIBRATE 1
#define MENU_ROW_TIMERS_HOURS 2

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index , void *data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index , void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void vibration_callback(TimerVibration vibration);

static Window* window;
static MenuLayer* layer_menu;

void win_settings_init(void) {
  window = window_create();

  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
}

void win_settings_show(void) {
  window_stack_push(window, true);
}

void win_settings_destroy(void) {
  layer_remove_from_parent(menu_layer_get_layer(layer_menu));
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

//----------------------------------------------------------------------------//

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return MENU_NUM_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_SAVE:
      return MENU_SECTION_ROWS_SAVE;
    break;
    case MENU_SECTION_TIMERS:
      return MENU_SECTION_ROWS_TIMERS;
    break;
  }
  return 0;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 36;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  char label[32] = "";
  char value[16] = "";

  switch (cell_index->section) {
    case MENU_SECTION_SAVE:
      switch (cell_index->row) {
        case MENU_ROW_SAVE_AUTO:
          strcpy(label, "Auto Save");
          strcpy(value, settings()->save_timers_auto ? "ON": "OFF");
        break;
        case MENU_ROW_SAVE_RESUME:
          strcpy(label, "Auto Resume");
          strcpy(value, settings()->resume_timers ? "ON": "OFF");
        break;
      }
    break;
    case MENU_SECTION_TIMERS:
      switch (cell_index->row) {
        case MENU_ROW_TIMERS_START:
          strcpy(label, "Auto-Start");
          strcpy(value, settings()->timers_start_auto ? "ON": "OFF");
        break;
        case MENU_ROW_TIMERS_VIBRATE:
          strcpy(label, "Vibration");
          strcpy(value, timer_vibe_str(settings()->timers_vibration, true));
          uppercase(value);
        break;
        case MENU_ROW_TIMERS_HOURS:
          strcpy(label, "Hours");
          strcpy(value, settings()->timers_hours ? "ON": "OFF");
        break;
      }
    break;
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 2, 136, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  if (strlen(value) > 0) {
    graphics_draw_text(ctx, value, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(4, 6, 136, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
  }
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index , void *data) {
  switch (section_index) {
    case MENU_SECTION_SAVE:
      menu_cell_basic_header_draw(ctx, cell_layer, "Save Options");
    break;
    case MENU_SECTION_TIMERS:
      menu_cell_basic_header_draw(ctx, cell_layer, "Timer Defaults");
    break;
  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_SAVE:
      switch (cell_index->row) {
        case MENU_ROW_SAVE_AUTO:
          settings()->save_timers_auto = ! settings()->save_timers_auto;
          menu_layer_reload_data(layer_menu);
        break;
        case MENU_ROW_SAVE_RESUME:
          settings()->resume_timers = ! settings()->resume_timers;
          menu_layer_reload_data(layer_menu);
        break;
      }
    break;
    case MENU_SECTION_TIMERS:
      switch (cell_index->row) {
        case MENU_ROW_TIMERS_START:
          settings()->timers_start_auto = ! settings()->timers_start_auto;
          menu_layer_reload_data(layer_menu);
        break;
        case MENU_ROW_TIMERS_VIBRATE:
          win_vibration_show(vibration_callback, settings()->timers_vibration);
        break;
        case MENU_ROW_TIMERS_HOURS:
          settings()->timers_hours = ! settings()->timers_hours;
          menu_layer_reload_data(layer_menu);
        break;
      }
    break;
  }
}

static void vibration_callback(TimerVibration vibration) {
  settings()->timers_vibration = vibration;
}