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

src/windows/win-timer.c

*/

#include <pebble.h>
#include "../timer.h"
#include "../common.h"
#include "../timers.h"
#include "win-add.h"
#include <pebble-assist.h>
#include <bitmap-loader.h>

#define HEIGHT_TOP        36
#define HEIGHT_TOP_LABEL  50

#define FRAME_TOP         GRect(0, 0, PEBBLE_WIDTH, HEIGHT_TOP)
#define FRAME_TOP_LABEL   GRect(0, 0, PEBBLE_WIDTH, HEIGHT_TOP_LABEL)
#define FRAME_MENU        GRect(0, HEIGHT_TOP, PEBBLE_WIDTH, PEBBLE_HEIGHT - STATUS_HEIGHT - HEIGHT_TOP)
#define FRAME_MENU_LABEL  GRect(0, HEIGHT_TOP_LABEL, PEBBLE_WIDTH, PEBBLE_HEIGHT - STATUS_HEIGHT - HEIGHT_TOP_LABEL)

#define MENU_ROW_START_PAUSE 0
#define MENU_ROW_RESET 1
#define MENU_ROW_CLEAR 2
#define MENU_ROW_EDIT 3

static void window_load(Window* window);
static void window_unload(Window* window);
static void window_appear(Window* window);
static void window_disappear(Window* window);

static void ticker_callback(void* data);

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);

static void layer_timer_update(Layer* layer, GContext* ctx);

static Window* window;
static MenuLayer* menu_layer;
static InverterLayer* layer_invert;
static Layer* layer_timer;
static Timer* timer = NULL;
static uint8_t timer_pos = -1;
static AppTimer* ticker = NULL;

void win_timer_init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear,
    .disappear = window_disappear
  });
}

void win_timer_show(Timer* tmr, uint8_t pos) {
  timer = tmr;
  timer_pos = pos;
  window_stack_push(window, ANIMATE_WINDOWS);
  if (NULL == layer_timer) {
    return;
  }
  if (strlen(timer->label) > 0) {
    layer_set_frame(layer_timer, FRAME_TOP_LABEL);
    layer_set_frame(inverter_layer_get_layer(layer_invert), FRAME_TOP_LABEL);
    layer_set_frame(menu_layer_get_layer(menu_layer), FRAME_MENU_LABEL);
  }
  else {
    layer_set_frame(layer_timer, FRAME_TOP);
    layer_set_frame(inverter_layer_get_layer(layer_invert), FRAME_TOP);
    layer_set_frame(menu_layer_get_layer(menu_layer), FRAME_MENU);
  }
}

void win_timer_destroy(void) {
  window_destroy(window);
}

//----------------------------------------------------------------------------//

static void window_load(Window* window) {

  bool label = false;
  if (timer && strlen(timer->label) > 0) {
    label = true;
  }

  menu_layer = menu_layer_create(label ? FRAME_MENU_LABEL : FRAME_MENU);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback
  });
  menu_layer_add_to_window(menu_layer, window);

  layer_timer = layer_create(label ? FRAME_TOP_LABEL : FRAME_TOP);
  layer_set_update_proc(layer_timer, layer_timer_update);
  layer_add_to_window(layer_timer, window);

  layer_invert = inverter_layer_create(label ? FRAME_TOP_LABEL : FRAME_TOP);
  inverter_layer_add_to_window(layer_invert, window);
}

static void window_unload(Window* window) {
  inverter_layer_destroy_safe(layer_invert);
  layer_destroy_safe(layer_timer);
  menu_layer_destroy_safe(menu_layer);
}

static void window_appear(Window* window) {
  ticker_callback(NULL);
}

static void window_disappear(Window* window) {
  app_timer_cancel_safe(ticker);
}

static void ticker_callback(void* data) {
  layer_mark_dirty(layer_timer);
  menu_layer_reload_data(menu_layer);
  ticker = app_timer_register(1000, ticker_callback, NULL);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return timer->status == TIMER_STATUS_STOPPED ? 4 : 3;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 32;
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_START_PAUSE: {
      switch (timer->status) {
        case TIMER_STATUS_RUNNING:
          timer_pause(timer);
        break;
        case TIMER_STATUS_PAUSED:
          timer_resume(timer);
        break;
        case TIMER_STATUS_FINISHED:
          timer_reset(timer);
          timer_start(timer);
        break;
        case TIMER_STATUS_STOPPED:
          timer_start(timer);
        break;
      }
    }
    break;
    case MENU_ROW_RESET:
      timer_reset(timer);
    break;
    case MENU_ROW_CLEAR:
      timers_remove(timer_pos);
      window_stack_pop(ANIMATE_WINDOWS);
      timers_send_list();
      return;
    break;
    case MENU_ROW_EDIT:
      win_add_show_edit(timer);
    break;
  }
  menu_layer_reload_data(menu_layer);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENU_ROW_START_PAUSE: {
      switch (timer->status) {
        case TIMER_STATUS_FINISHED:
        case TIMER_STATUS_STOPPED:
          draw_icon_text_row(ctx, "Start", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY));
        break;
        case TIMER_STATUS_PAUSED:
          draw_icon_text_row(ctx, "Resume", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY));
        break;
        case TIMER_STATUS_RUNNING:
          draw_icon_text_row(ctx, "Pause", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE));
        break;
      }
    }
    break;
    case MENU_ROW_RESET:
      draw_icon_text_row(ctx, "Reset", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_RESET));
    break;
    case MENU_ROW_CLEAR:
      draw_icon_text_row(ctx, "Destroy", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLEAR));
    break;
    case MENU_ROW_EDIT:
      draw_icon_text_row(ctx, "Edit", bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_EDIT));
    break;
  }
}

static void layer_timer_update(Layer* layer, GContext* ctx) {
  timer_draw(timer, ctx);
}
