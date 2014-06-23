/*

Multi Timer v2.7.1
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

src/windows/win-add.c

*/

#include <pebble.h>

#include "../libs/pebble-assist/pebble-assist.h"
#include "win-timers.h"
#include "win-add-vibration.h"
#include "win-add-duration.h"
#include "../timer.h"
#include "../timers.h"
#include "../settings.h"
#include "../common.h"

#define MENU_SECTION_MAIN 0
#define MENU_SECTION_FOOTER 1

#define MENU_ROW_DIRECTION 0
#define MENU_ROW_DURATION 1
#define MENU_ROW_VIBRATION 2
#define MENU_ROW_REPEAT 3

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window;
static MenuLayer* layer_menu;
static Timer* timer = NULL;

void win_add_init(void) {
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

  win_add_vibration_init();
  win_add_duration_init();
}

void win_add_show(void) {
  // TODO: Fix the rather bad memory leak that will occur here.
  timer = malloc(sizeof(Timer));
  timer->direction = TIMER_DIRECTION_DOWN;
  timer->vibrate = settings()->timers_vibration;
  timer->length = 10 * 60;
  timer->repeat = false;
  timer->label[0] = 0;

  window_stack_push(window, true);
  menu_layer_reload_data(layer_menu);
  menu_layer_set_selected_index(layer_menu, MenuIndex(0, 0), MenuRowAlignTop, false);
}

void win_add_destroy(void) {
  win_add_vibration_destroy();
  win_add_duration_destroy();
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 2;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      if (timer == NULL) {
        return 0;
      }
      if (timer->direction == TIMER_DIRECTION_DOWN) {
        return 4;
      }
      return 1;
    break;
    case MENU_SECTION_FOOTER:
      return 1;
    break;
  }
  return 1;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_FOOTER:
      return 4;
    break;
  }
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  switch (cell_index->section) {
    case MENU_SECTION_FOOTER:
      return 44;
    break;
  }
  return 36;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  return;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  switch (cell_index->section) {

    case MENU_SECTION_MAIN: {

      char option[16];
      char value[16];

      switch (cell_index->row) {
        case MENU_ROW_DIRECTION:
          strcpy(option, "Count");
          strcpy(value, timer->direction == TIMER_DIRECTION_UP ?  "Up" : "Down");
          uppercase(value);
        break;
        case MENU_ROW_DURATION:
          strcpy(option, "Duration");
          timer_duration_str(timer->length, settings()->timers_hours, value, sizeof(value));
        break;
        case MENU_ROW_VIBRATION:
          strcpy(option, "Vibration");
          switch (timer->vibrate) {
            case TIMER_VIBRATION_OFF:
              strcpy(value, "None");
            break;
            case TIMER_VIBRATION_SHORT:
              strcpy(value, "Short");
            break;
            case TIMER_VIBRATION_LONG:
              strcpy(value, "Long");
            break;
            case TIMER_VIBRATION_DOUBLE:
              strcpy(value, "Double");
            break;
            case TIMER_VIBRATION_TRIPLE:
              strcpy(value, "Triple");
            break;
            case TIMER_VIBRATION_CONTINUOUS:
              strcpy(value, "Solid");
            break;
          }
          uppercase(value);
        break;
        case MENU_ROW_REPEAT:
          strcpy(option, "Repeat");
          strcpy(value, timer->repeat ? "On" : "Off");
          uppercase(value);
        break;
      }

      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_draw_text(ctx, option, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 2, 136, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
      graphics_draw_text(ctx, value, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(4, 6, 136, 20), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
    }
    break;

    case MENU_SECTION_FOOTER:
      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_draw_text(ctx, "Add Timer", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 5, 136, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    break;

  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      switch (cell_index->row) {
        case MENU_ROW_DIRECTION:
          timer->direction = timer->direction == TIMER_DIRECTION_UP ? TIMER_DIRECTION_DOWN : TIMER_DIRECTION_UP;
          menu_layer_reload_data(layer_menu);
        break;
        case MENU_ROW_DURATION:
          win_add_duration_show(timer);
        break;
        case MENU_ROW_VIBRATION:
          win_add_vibration_show(timer);
        break;
        case MENU_ROW_REPEAT:
          timer->repeat = ! timer->repeat;
          menu_layer_reload_data(layer_menu);
        break;
      }
    break;
    case MENU_SECTION_FOOTER:
      if (timer->direction == TIMER_DIRECTION_DOWN && timer->length == 0) {
        vibes_short_pulse();
        menu_layer_set_selected_index(layer_menu, (MenuIndex) { MENU_SECTION_MAIN, MENU_ROW_DURATION }, MenuRowAlignCenter, true);
        return;
      }
      timer_init(timer);
      timers_add(timer);
      if (settings()->timers_start_auto) {
        timer_start(timer);
      }
      win_timers_jump(timers_get_count() - 1);
      window_stack_pop(true);
    break;
  }
}