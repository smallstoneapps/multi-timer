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

src/windows/win-timer.c

*/

#include <pebble.h>
#include <pebble-assist.h>
#include <bitmap-loader.h>
#include "win-timer.h"
#include "win-timer-add.h"
#include "../timer.h"
#include "../common.h"
#include "../icons.h"
#include "../timers.h"

#define MENU_ROW_PAUSE  0
#define MENU_ROW_RESET  1
#define MENU_ROW_DELETE 2
#define MENU_ROW_EDIT   3

static void window_load(Window* window);
static void window_unload(Window* window);
static void layer_header_update(Layer* layer, GContext* ctx);

static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context);
static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context);
static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context);
static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context);

static void timers_update_handler(void);
static bool can_edit(void);

static Window* s_window;
static Timer* s_timer;
static Layer* s_layer_header;
static InverterLayer* s_layer_header_invert;
static MenuLayer* s_layer_menu;

void win_timer_init(void) {
  s_timer = NULL;
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  timers_register_update_handler(timers_update_handler);
}

void win_timer_set_timer(Timer* timer) {
  s_timer = timer;
}

void win_timer_show(void) {
  if (! s_timer) {
    return;
  }
  window_stack_push(s_window, false);
}

static void window_load(Window* window) {
  s_layer_header = layer_create(GRect(0, 0, PEBBLE_WIDTH, 36));
  layer_set_update_proc(s_layer_header, layer_header_update);
  layer_add_to_window(s_layer_header, s_window);

  s_layer_header_invert = inverter_layer_create(layer_get_frame(s_layer_header));
  inverter_layer_add_to_window(s_layer_header_invert, s_window);

  s_layer_menu = menu_layer_create(GRect(0, 36, PEBBLE_WIDTH, PEBBLE_HEIGHT - STATUS_HEIGHT - 36));
  menu_layer_set_callbacks(s_layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_num_sections,
    .get_num_rows = menu_num_rows,
    .get_cell_height = menu_cell_height,
    .draw_row = menu_draw_row,
    .select_click = menu_select,
  });
  menu_layer_add_to_window(s_layer_menu, s_window);
}

static void window_unload(Window* window) {
  menu_layer_destroy(s_layer_menu);
  inverter_layer_destroy(s_layer_header_invert);
  layer_destroy(s_layer_header);
}

static void layer_header_update(Layer* layer, GContext* ctx) {
  timer_draw_row(s_timer, ctx);
}

static uint16_t menu_num_sections(struct MenuLayer* menu, void* callback_context) {
  return 1;
}

static uint16_t menu_num_rows(struct MenuLayer* menu, uint16_t section_index, void* callback_context) {
  return can_edit() ? 4 : 3;
}

static int16_t menu_cell_height(struct MenuLayer *menu, MenuIndex *cell_index, void *callback_context) {
  return 32;
}

static void menu_draw_row(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_PAUSE: {
      switch (s_timer->status) {
        case TIMER_STATUS_RUNNING:
          menu_draw_row_icon_text(ctx, "Pause", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_PAUSE));
          break;
        case TIMER_STATUS_PAUSED:
          menu_draw_row_icon_text(ctx, "Resume", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_PLAY));
          break;
        case TIMER_STATUS_DONE:
        case TIMER_STATUS_STOPPED:
          menu_draw_row_icon_text(ctx, "Start", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_PLAY));
          break;
      }
      break;
    }
    case MENU_ROW_RESET:
      menu_draw_row_icon_text(ctx, "Reset", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_RESET));
      break;
    case MENU_ROW_DELETE:
      menu_draw_row_icon_text(ctx, "Delete", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_DELETE));
      break;
    case MENU_ROW_EDIT:
      menu_draw_row_icon_text(ctx, "Edit", bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_EDIT));
      break;
  }
}

static void menu_select(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_PAUSE: {
      switch (s_timer->status) {
        case TIMER_STATUS_RUNNING:
          timer_pause(s_timer);
          break;
        case TIMER_STATUS_PAUSED:
          timer_resume(s_timer);
          break;
        case TIMER_STATUS_DONE:
        case TIMER_STATUS_STOPPED:
          timer_start(s_timer);
          break;
      }
      break;
    }
    case MENU_ROW_RESET:
      timer_reset(s_timer);
      break;
    case MENU_ROW_DELETE:
      timers_remove(timers_index_of(s_timer->id));
      window_stack_pop(false);
      break;
    case MENU_ROW_EDIT:
      win_timer_add_show_edit(s_timer);
      break;
  }
}

static void timers_update_handler(void) {
  if (! window_is_loaded(s_window)) {
    return;
  }
  layer_mark_dirty(s_layer_header);
  menu_layer_reload_data(s_layer_menu);
}

static bool can_edit(void) {
  if (s_timer->type == TIMER_TYPE_STOPWATCH) {
    return false;
  }
  if (s_timer->status == TIMER_STATUS_STOPPED) {
    return true;
  }
  return false;
}