/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * win-add.c
 ***/

#include <pebble_os.h>
#include <pebble_fonts.h>
#include "win-timers.h"
#include "win-add-vibration.h"
#include "win-add-duration.h"
#include "timer.h"
#include "timers.h"

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

  layer_menu = menu_layer_create(layer_get_frame(window_get_root_layer(window)));
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

  win_add_vibration_init();
  win_add_duration_init();
}

void win_add_show(void) {
  timer = malloc(sizeof(Timer));
  timer->direction = TIMER_DOWN;
  timer->vibrate = TIMER_VIBE_OFF;
  timer->length = 10 * 60;

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
      if (timer->direction == TIMER_DOWN) {
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
      return 40;
    break;
  }
  return 34;
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
          strcpy(option, "Direction");
          strcpy(value, timer->direction == TIMER_UP ?  "Up" : "Down");
        break;
        case MENU_ROW_DURATION:
          strcpy(option, "Duration");
          snprintf(value, sizeof(value), "%02d:%02d", timer->length / 60, timer->length % 60);
        break;
        case MENU_ROW_VIBRATION:
          strcpy(option, "Vibration");
          switch (timer->vibrate) {
            case TIMER_VIBE_OFF:
              strcpy(value, "Off");
            break;
            case TIMER_VIBE_SHORT:
              strcpy(value, "Short");
            break;
            case TIMER_VIBE_LONG:
              strcpy(value, "Long");
            break;
            case TIMER_VIBE_DOUBLE:
              strcpy(value, "Double");
            break;
            case TIMER_VIBE_TRIPLE:
              strcpy(value, "Triple");
            break;
            case TIMER_VIBE_CONTINUOUS:
              strcpy(value, "Solid");
            break;
          }
        break;
        case MENU_ROW_REPEAT:
          strcpy(option, "Repeat");
          strcpy(value, timer->repeat ? "On" : "Off");
        break;
      }

      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_text_draw(ctx, option, fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(4, 0, 136, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
      graphics_text_draw(ctx, value, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 0, 136, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    }
    break;

    case MENU_SECTION_FOOTER:
      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_text_draw(ctx, "Add Timer", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 3, 136, 24), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    break;

  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      switch (cell_index->row) {
        case MENU_ROW_DIRECTION:
          timer->direction = timer->direction == TIMER_UP ? TIMER_DOWN : TIMER_UP;
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
      timers_add(timer);
      window_stack_pop(true);
    break;
  }
}