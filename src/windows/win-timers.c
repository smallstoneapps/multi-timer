/*

Multi Timer v2.7.0
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

src/windows/win-timers.c

*/

#include <pebble.h>

#include "../libs/pebble-assist/pebble-assist.h"
#include "../libs/bitmap-loader/bitmap-loader.h"
#include "../timers.h"
#include "../settings.h"
#include "../timer.h"
#include "../common.h"
#include "win-add.h"
#include "win-controls.h"
#include "win-settings.h"
#include "win-vibrate.h"
#include "win-about.h"
#include "win-timer.h"

#define MENU_SECTIONS 2
#define MENU_SECTION_TIMERS 0
#define MENU_SECTION_FOOTER 1

#define MENU_ROWS_FOOTER 4

#define MENU_ITEM_FOOTER_ADD 0
#define MENU_ITEM_FOOTER_CONTROLS 1
#define MENU_ITEM_FOOTER_SETTINGS 2
#define MENU_ITEM_FOOTER_ABOUT 3

#define ROW_HEIGHT 36

static void window_load(Window* window);
static void window_unload(Window* window);
static void window_appear(Window* window);

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_footer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void jump_to_timer(int t, bool animate);

static Window* window;
static MenuLayer* layer_menu;

void win_timers_init(void) {
  // Create the window and setup event handlers.
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear
  });
  // Initialise all of the windows that can be spawned from this one.
  win_add_init();
  win_controls_init();
  win_settings_init();
  win_vibrate_init();
  win_timer_init();
  win_about_init();
}

void win_timers_show(void) {
  window_stack_push(window, true);
}

void win_timers_update(void) {
  menu_layer_reload_data(layer_menu);
}

void win_timers_destroy(void) {
  win_vibrate_destroy();
  win_add_destroy();
  win_controls_destroy();
  win_settings_destroy();
  win_about_destroy();
  win_timer_destroy();
  window_destroy(window);
}

void win_timers_jump(int pos) {
  jump_to_timer(pos, false);
}

//----------------------------------------------------------------------------//

static void window_load(Window* window) {
  // Create the menu with callbacks and click config.
  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
    .select_long_click = menu_select_long_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
}

static void window_unload(Window* window) {
  menu_layer_destroy(layer_menu);
}

static void window_appear(Window* window) {
  menu_layer_reload_data(layer_menu);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_FOOTER:
      return MENU_ROWS_FOOTER;
    break;
    case MENU_SECTION_TIMERS:
      return timers_get_count();
    break;
  }
  return 0;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  if (timers_get_count() == 0) {
    return 0;
  }
  switch (section_index) {
    case MENU_SECTION_FOOTER:
      return 4;
    break;
    case MENU_SECTION_TIMERS:
      return 0;
    break;
  }
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  switch (cell_index->section) {
    case MENU_SECTION_TIMERS:
      return ROW_HEIGHT + 4;
    break;
    case MENU_SECTION_FOOTER:
      return ROW_HEIGHT;
    break;
  }
  return 0;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_FOOTER:
    break;
    case MENU_SECTION_TIMERS:
    break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case MENU_SECTION_FOOTER:
      menu_draw_footer_row(ctx, cell_layer, cell_index, data);
    break;
    case MENU_SECTION_TIMERS:
      menu_draw_timer_row(ctx, cell_layer, cell_index, data);
    break;
  }
}

static void menu_draw_footer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  char* row_label = malloc(20);
  GBitmap* row_icon = NULL;

  switch (cell_index->row) {
    case MENU_ITEM_FOOTER_ADD:
      strcpy(row_label, "Add Timer");
      row_icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_ADD);
    break;
    case MENU_ITEM_FOOTER_CONTROLS:
      strcpy(row_label, "Controls");
      row_icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_CONTROLS);
    break;
    case MENU_ITEM_FOOTER_SETTINGS:
      strcpy(row_label, "Settings");
      row_icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_SETTINGS);
    break;
    case MENU_ITEM_FOOTER_ABOUT:
      strcpy(row_label, "About");
      row_icon = bitmaps_get_bitmap(RESOURCE_ID_MENU_ICON_ABOUT);
    break;
  }

  graphics_context_set_text_color(ctx, GColorBlack);
  if (row_icon != NULL) {
    graphics_draw_bitmap_in_rect(ctx, row_icon, GRect(8, 8, 20, 20));
  }
  graphics_draw_text(ctx, row_label,
    fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
    GRect(36, 1, 112, 28), 0, GTextAlignmentLeft, NULL);

  free(row_label);
}

static void menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  Timer* timer = timers_get(cell_index->row);
  timer_draw(timer, ctx);
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_FOOTER: {
      switch (cell_index->row) {
        case MENU_ITEM_FOOTER_ADD:
          win_add_show();
          break;
        case MENU_ITEM_FOOTER_CONTROLS:
          win_controls_show();
          break;
        case MENU_ITEM_FOOTER_SETTINGS:
          win_settings_show();
          break;
        case MENU_ITEM_FOOTER_ABOUT:
          win_about_show(true);
          break;
      }
    }
    break;
    case MENU_SECTION_TIMERS: {
      if (timers_get_count() == 0) {
        return;
      }

      Timer* timer = timers_get(cell_index->row);
      if (! timer) {
        return;
      }
      switch (timer->status) {
        case TIMER_STATUS_RUNNING:
          timer_pause(timer);
        break;
        case TIMER_STATUS_PAUSED:
          timer_resume(timer);
        break;
        case TIMER_STATUS_STOPPED:
          timer_start(timer);
        break;
        case TIMER_STATUS_FINISHED:
          timer_reset(timer);
        break;
      }
    }
    break;
  }
  menu_layer_reload_data(layer_menu);
}

static void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if (cell_index->section != MENU_SECTION_TIMERS) {
    return;
  }
  Timer* timer = timers_get(cell_index->row);
  win_timer_set_timer(timer, cell_index->row);
  win_timer_show();
}

static void jump_to_timer(int t, bool animate) {
  MenuIndex index =  { MENU_SECTION_TIMERS, t };
  menu_layer_set_selected_index(layer_menu, index, MenuRowAlignCenter, animate);
}