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

src/windows/win-controls.c

*/

#include <pebble.h>
#include <pebble-assist.h>
#include <bitmap-loader.h>
#include "win-controls.h"
#include "../timers.h"
#include "../timer.h"
#include "../icons.h"
#include "../common.h"

#define MENU_ROW_RESUME 0
#define MENU_ROW_PAUSE  1
#define MENU_ROW_RESET  2
#define MENU_ROW_DELETE 3

static void window_load(Window* window);
static void window_unload(Window* window);
static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context);
static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context);
static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context);
static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context);

static Window* s_window;
static MenuLayer* s_menu;

void win_controls_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
}

void win_controls_show(void) {
  window_stack_push(s_window, true);
}

static void window_load(Window* window) {
  s_menu = menu_layer_create_fullscreen(s_window);
  menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_num_sections,
    .get_num_rows = menu_num_rows,
    .get_cell_height = menu_cell_height,
    .draw_row = menu_draw_row,
    .select_click = menu_select,
  });
  menu_layer_add_to_window(s_menu, s_window);
}

static void window_unload(Window* window) {
  menu_layer_destroy(s_menu);
}

static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context) {
  return 1;
}

static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context) {
  return 4;
}

static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context) {
  return 32;
}

static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_RESUME:
      menu_draw_row_icon_text(ctx, "Resume All", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_PLAY));
      break;
    case MENU_ROW_PAUSE:
      menu_draw_row_icon_text(ctx, "Pause All", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_PAUSE));
      break;
    case MENU_ROW_RESET:
      menu_draw_row_icon_text(ctx, "Reset All", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_RESET));
      break;
    case MENU_ROW_DELETE:
      menu_draw_row_icon_text(ctx, "Delete All", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_DELETE));
      break;
  }
}

static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context) {
  uint8_t num_timers = timers_count();
  switch (cell_index->row) {
    case MENU_ROW_RESUME:
      for (uint8_t t = 0; t < num_timers; t += 1) {
        Timer* timer = timers_get(t);
        switch (timer->status) {
          case TIMER_STATUS_RUNNING:
            break;
          case TIMER_STATUS_PAUSED:
            timer_resume(timer);
            break;
          case TIMER_STATUS_DONE:
            timer_reset(timer);
            timer_start(timer);
            break;
          case TIMER_STATUS_STOPPED:
            timer_start(timer);
            break;
        }
      }
      break;
    case MENU_ROW_PAUSE:
      for (uint8_t t = 0; t < num_timers; t += 1) {
        Timer* timer = timers_get(t);
        if (timer->status == TIMER_STATUS_RUNNING) {
          timer_pause(timer);
        }
      }
      break;
    case MENU_ROW_RESET:
      for (uint8_t t = 0; t < num_timers; t += 1) {
        timer_reset(timers_get(t));
      }
      break;
    case MENU_ROW_DELETE:
      for (uint8_t t = 0; t < num_timers; t += 1) {
        timers_remove(0);
      }
      break;
  }
  window_stack_pop(true);
}
