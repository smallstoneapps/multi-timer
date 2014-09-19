/*

Multi Timer v2.8.0

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

src/windows/win-add.c

*/

#include <pebble.h>

#include <pebble-assist.h>
#include "win-main.h"
#include "win-add-duration.h"
#include "win-vibration.h"
#include "../timer.h"
#include "../timers.h"
#include "../settings.h"
#include "../common.h"

#define MENU_SECTION_MAIN   0
#define MENU_SECTION_FOOTER 1

#define MENU_ROW_DIRECTION  0
#define MENU_ROW_DURATION   1
#define MENU_ROW_VIBRATION  2
#define MENU_ROW_REPEAT     3

static void window_load(Window* window);
static void window_unload(Window* window);
static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_draw_row_main(GContext* ctx, uint16_t row);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_main(uint16_t row);
static void menu_select_footer(void);
static void vibration_callback(TimerVibration vibration);

static Window* window;
static MenuLayer* layer_menu;
static Timer* timer = NULL;
static bool edit_mode = false;

void win_add_init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  win_add_duration_init();
}

void win_add_show_new(void) {
  free_safe(timer);
  timer = timer_create();
  window_stack_push(window, ANIMATE_WINDOWS);
  edit_mode = false;
}

void win_add_show_edit(Timer* tmr) {
  free_safe(timer);
  timer = timer_clone(tmr);
  window_stack_push(window, ANIMATE_WINDOWS);
  edit_mode = true;
}

void win_add_destroy(void) {
  win_add_duration_destroy();
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void window_load(Window* window) {
  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections   = menu_get_num_sections_callback,
    .get_num_rows       = menu_get_num_rows_callback,
    .get_header_height  = menu_get_header_height_callback,
    .get_cell_height    = menu_get_cell_height_callback,
    .draw_header        = menu_draw_header_callback,
    .draw_row           = menu_draw_row_callback,
    .select_click       = menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
}

static void window_unload(Window* window) {
  menu_layer_destroy(layer_menu);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 2;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      if (timer == NULL) {
        return 0;
      }
      if (timer->direction == TIMER_DIRECTION_DOWN) {
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
      return 44;
    break;
  }
  return 32;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  return;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      menu_draw_row_main(ctx, cell_index->row);
      break;
    case MENU_SECTION_FOOTER:
      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_draw_text(ctx, edit_mode ? "Save Timer" : "Add Timer", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 5, 136, 28), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      break;
  }
}

static void menu_draw_row_main(GContext* ctx, uint16_t row) {
  char tmp[16];
  switch (row) {
    case MENU_ROW_DIRECTION:
      menu_draw_option(ctx, "Count", timer->direction == TIMER_DIRECTION_UP ?  "UP" : "DOWN");
      break;
    case MENU_ROW_DURATION:
      timer_duration_str(timer->length, settings()->timers_hours, tmp, sizeof(tmp));
      menu_draw_option(ctx, "Duration", tmp);
      break;
    case MENU_ROW_VIBRATION:
      strcpy(tmp, timer_vibe_str(timer->vibrate, true));
      uppercase(tmp);
      menu_draw_option(ctx, "Vibration", tmp);
      break;
    case MENU_ROW_REPEAT:
      menu_draw_option(ctx, "Repeat", timer->repeat ? "ON" : "OFF");
      break;
  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      menu_select_main(cell_index->row);
      break;
    case MENU_SECTION_FOOTER:
      menu_select_footer();
    break;
  }
}

static void menu_select_main(uint16_t row) {
  switch (row) {
    case MENU_ROW_DIRECTION:
      timer->direction = timer->direction == TIMER_DIRECTION_UP ? TIMER_DIRECTION_DOWN : TIMER_DIRECTION_UP;
      menu_layer_reload_data(layer_menu);
    break;
    case MENU_ROW_DURATION:
      win_add_duration_show(timer);
    break;
    case MENU_ROW_VIBRATION:
      win_vibration_show(vibration_callback, timer->vibrate);
    break;
    case MENU_ROW_REPEAT:
      timer->repeat = ! timer->repeat;
      menu_layer_reload_data(layer_menu);
    break;
  }
}

static void menu_select_footer(void) {
  if (timer->direction == TIMER_DIRECTION_DOWN && timer->length == 0) {
    vibes_short_pulse();
    menu_layer_set_selected_index(layer_menu, (MenuIndex) { MENU_SECTION_MAIN, MENU_ROW_DURATION }, MenuRowAlignCenter, true);
    return;
  }
  if (edit_mode) {
    Timer* tmr = timers_find(timer->id);
    tmr->direction = timer->direction;
    tmr->length = timer->length;
    tmr->vibrate = timer->vibrate;
    tmr->repeat = timer->repeat;
    timer_init(tmr);
  }
  else {
    timer_init(timer);
    timers_add(timer_clone(timer));
    if (settings()->timers_start_auto) {
      timer_start(timer);
    }
    win_main_jump(timers_get_count() - 1);
  }
  timers_send_list();
  window_stack_pop(false);
}

static void vibration_callback(TimerVibration vibration) {
  timer->vibrate = vibration;
}