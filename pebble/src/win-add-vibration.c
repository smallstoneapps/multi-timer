/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * win-add-vibration.c
 ***/

#include <pebble_os.h>
#include <pebble_fonts.h>
#include <pebble_app.h>
#include "win-add-vibration.h"
#include "timer.h"
#include "bitmaps.h"

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window;
static MenuLayer* layer_menu;
static Timer* timer = NULL;

void win_add_vibration_init(void) {
  window = window_create();

  layer_menu = menu_layer_create(layer_get_bounds(window_get_root_layer(window)));
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(layer_menu));
}

void win_add_vibration_show(Timer* tmr) {
  window_stack_push(window, true);
  timer = tmr;
}

void win_add_vibration_destroy(void) {
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 6;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 32;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  if (timer == NULL) {
    return;
  }

  char label[24];

  switch (cell_index->row) {
    case TIMER_VIBE_OFF:
      strcpy(label, "Off");
    break;
    case TIMER_VIBE_SHORT:
      strcpy(label, "Short Pulse");
    break;
    case TIMER_VIBE_LONG:
      strcpy(label, "Long Pulse");
    break;
    case TIMER_VIBE_DOUBLE:
      strcpy(label, "Double Pulse");
    break;
    case TIMER_VIBE_TRIPLE:
      strcpy(label, "Triple Pulse");
    break;
    case TIMER_VIBE_CONTINUOUS:
      strcpy(label, "Continous");
    break;
  }

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_text_draw(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, -1, 112, 24), GTextOverflowModeTrailingEllipsis , GTextAlignmentLeft, NULL);
  if (timer->vibrate == cell_index->row) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_MED_TICK), GRect(120, 8, 16, 16));
  }

}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  timer->vibrate = cell_index->row;
  window_stack_pop_all(true);
}