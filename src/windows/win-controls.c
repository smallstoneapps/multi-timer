/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * win-controls.c
 ***/

#include <pebble.h>

#include "../libs/pebble-assist/pebble-assist.h"
#include "../libs/bitmap-loader/bitmap-loader.h"
#include "win-timers.h"
#include "win-controls.h"
#include "../timers.h"
#include "../timer.h"

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
  return 36;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GBitmap* icon = NULL;
  char* label = malloc(32);

  switch (cell_index->row) {
    case MENU_ROW_START:
      strcpy(label, "Start All");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PLAY);
    break;
    case MENU_ROW_PAUSE:
      strcpy(label, "Pause All");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_PAUSE);
    break;
    case MENU_ROW_RESET:
      strcpy(label, "Reset All");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_RESET);
    break;
    case MENU_ROW_CLEAR:
      strcpy(label, "Clear Timers");
      icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CLEAR);
    break;
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(36, 2, 104, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  if (icon != NULL) {
    graphics_draw_bitmap_in_rect(ctx, icon, GRect(8, 6, 24, 24));
  }
  free(label);
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
    break;
    case MENU_ROW_CLEAR:
      timers_clear();
    break;
  }
  window_stack_pop(true);
}
