/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * win-settings.c
 ***/

#include <pebble_os.h>
#include <pebble_fonts.h>
#include "win-settings.h"
#include "settings.h"
#include "timers.h"

#define MENU_NUM_SECTIONS 3

#define MENU_SECTION_SAVE 0
#define MENU_SECTION_TIMERS 1
#define MENU_SECTION_MISC 2

#define MENU_SECTION_ROWS_SAVE 3
#define MENU_SECTION_ROWS_TIMERS 1
#define MENU_SECTION_ROWS_MISC 1

#define MENU_ROW_SAVE_AUTO 0
#define MENU_ROW_SAVE_SAVE 1
#define MENU_ROW_SAVE_LOAD 2
#define MENU_ROW_TIMERS_START 0
#define MENU_ROW_MISC_THANK 0

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index , void *data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index , void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window;
static MenuLayer* layer_menu;

void win_settings_init(void) {
  window = window_create();

  layer_menu = menu_layer_create(layer_get_bounds(window_get_root_layer(window)));
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
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(layer_menu));
}

void win_settings_show(void) {
  window_stack_push(window, true);
}

void win_settings_destroy(void) {
  layer_remove_from_parent(menu_layer_get_layer(layer_menu));
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

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
    case MENU_SECTION_MISC:
      return MENU_SECTION_ROWS_MISC;
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
        case MENU_ROW_SAVE_SAVE:
          strcpy(label, "Save Timers");
        break;
        case MENU_ROW_SAVE_LOAD:
          strcpy(label, "Load Timers");
        break;
      }
    break;
    case MENU_SECTION_TIMERS:
      switch (cell_index->row) {
        case MENU_ROW_TIMERS_START:
          strcpy(label, "Auto-Start");
          strcpy(value, settings()->timers_start_auto ? "ON": "OFF");
        break;
      }
    break;
    case MENU_SECTION_MISC:
      switch (cell_index->row) {
        case MENU_ROW_MISC_THANK:
          strcpy(label, "Thank the dev");
        break;
      }
    break;
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_text_draw(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 2, 136, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  if (strlen(value) > 0) {
    graphics_text_draw(ctx, value, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(108, 6, 32, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
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
    case MENU_SECTION_MISC:
      menu_cell_basic_header_draw(ctx, cell_layer, "Miscelleanous");
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
        case MENU_ROW_SAVE_SAVE:
          timers_save();
        break;
        case MENU_ROW_SAVE_LOAD:
          timers_restore();
        break;
      }
    break;
    case MENU_SECTION_TIMERS:
      switch (cell_index->row) {
        case MENU_ROW_TIMERS_START:
          settings()->timers_start_auto = ! settings()->timers_start_auto;
          menu_layer_reload_data(layer_menu);
        break;
      }
    break;
    case MENU_SECTION_MISC:
      switch (cell_index->row) {
        case MENU_ROW_MISC_THANK:
        break;
      }
    break;
  }
}