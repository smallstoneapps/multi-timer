/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * windows/win-timer.c
 ***/

#include <pebble.h>
#include "../timer.h"
#include "../timers.h"
#include "../libs/pebble-assist/pebble-assist.h"
#include "../libs/bitmap-loader/bitmap-loader.h"

#define FRAME_TOP GRect(0, 0, 144, 40)
#define FRAME_MENU GRect(0, 40, 144, 112)

#define MENU_ROW_STARTPAUSE 0
#define MENU_ROW_RESET 1
#define MENU_ROW_CLEAR 2

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

void win_timer_set_timer(Timer* tmr, uint8_t pos) {
  timer = tmr;
  timer_pos = pos;
}

void win_timer_show(void) {
  window_stack_push(window, true);
}

void win_timer_destroy(void) {
  window_destroy(window);
}

//

static void window_load(Window* window) {
  menu_layer = menu_layer_create(FRAME_MENU);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback
  });
  menu_layer_add_to_window(menu_layer, window);

  layer_timer = layer_create(FRAME_TOP);
  layer_set_update_proc(layer_timer, layer_timer_update);
  layer_add_to_window(layer_timer, window);

  layer_invert = inverter_layer_create(FRAME_TOP);
  inverter_layer_add_to_window(layer_invert, window);
}

static void window_unload(Window* window) {
  inverter_layer_destroy(layer_invert);
  layer_destroy(layer_timer);
  menu_layer_destroy(menu_layer);
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
  return 3;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 36;
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_STARTPAUSE: {
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
      window_stack_pop(true);
      return;
    break;
  }
  menu_layer_reload_data(menu_layer);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GBitmap* icon = NULL;
  char* label = malloc(32);

  switch (cell_index->row) {
    case MENU_ROW_STARTPAUSE: {
      switch (timer->status) {
        case TIMER_STATUS_FINISHED:
        case TIMER_STATUS_STOPPED:
          strcpy(label, "Start");
          icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY);
        break;
        case TIMER_STATUS_PAUSED:
          strcpy(label, "Resume");
          icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY);
        break;
        case TIMER_STATUS_RUNNING:
          strcpy(label, "Pause");
          icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE);
        break;
      }
    }
    break;
    case MENU_ROW_RESET:
      strcpy(label, "Reset");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_RESET);
    break;
    case MENU_ROW_CLEAR:
      strcpy(label, "Destroy");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLEAR);
    break;
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(36, 1, 104, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  if (icon != NULL) {
    graphics_draw_bitmap_in_rect(ctx, icon, GRect(8, 8, 20, 20));
  }
  free(label);
}

static void layer_timer_update(Layer* layer, GContext* ctx) {
  timer_draw(timer, ctx);
}