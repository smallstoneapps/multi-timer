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

src/windows/win-controls.c

*/

#include <pebble.h>

#include <pebble-assist.h>
#include <bitmap-loader.h>
#include "win-timers.h"
#include "win-controls.h"
#include "../timers.h"
#include "../timer.h"
#include "../common.h"

#define MENU_NUM_ROWS 4
#define MENU_ROW_START 0
#define MENU_ROW_PAUSE 1
#define MENU_ROW_RESET 2
#define MENU_ROW_CLEAR 3

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window;
static MenuLayer* layer_menu;

void win_controls_init(void) {
  window = window_create();

  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
}

void win_controls_show(void) {
  window_stack_push(window, true);
}

void win_controls_destroy(void) {
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return MENU_NUM_ROWS;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 32;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENU_ROW_START:
      draw_icon_text_row(ctx, "Start All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY));
    break;
    case MENU_ROW_PAUSE:
      draw_icon_text_row(ctx, "Pause All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE));
    break;
    case MENU_ROW_RESET:
      draw_icon_text_row(ctx, "Reset All", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_RESET));
    break;
    case MENU_ROW_CLEAR:
      draw_icon_text_row(ctx, "Clear Timers", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLEAR));
    break;
  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_START:
      for (int t = 0; t < timers_get_count(); t += 1) {
        if (timers_get(t)->status == TIMER_STATUS_PAUSED) {
          timer_resume(timers_get(t));
        }
        else if (timers_get(t)->status == TIMER_STATUS_STOPPED) {
          timer_start(timers_get(t));
        }
      }
      win_timers_jump(0);
    break;
    case MENU_ROW_PAUSE:
      for (int t = 0; t < timers_get_count(); t += 1) {
        timer_pause(timers_get(t));
      }
      win_timers_jump(0);
    break;
    case MENU_ROW_RESET:
      for (int t = 0; t < timers_get_count(); t += 1) {
        timer_reset(timers_get(t));
      }
      win_timers_jump(0);
      timers_send_list();
    break;
    case MENU_ROW_CLEAR:
      timers_clear();
    break;
  }
  window_stack_pop(true);
}
