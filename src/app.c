/*
 * Multi Timer
 * Copyright (C) 2013 Matthew Tole
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"
#include "timer.h"
#include "timers.h"
#include "add-window.h"
#include "vibe-window.h"

#if ROCKSHOT
#include "http.h"
#include "httpcapture.h"
#endif

#define MY_UUID { 0xBD, 0xB6, 0xA5, 0x5E, 0xB3, 0x2A, 0x4B, 0x03, 0xB0, 0x37, 0x95, 0x63, 0x7B, 0xF3, 0x06, 0xFF }

PBL_APP_INFO(MY_UUID, "Multi Timer", "Matthew Tole", 1, 2, RESOURCE_ID_MENU_ICON, APP_INFO_STANDARD_APP);

#define MENU_SECTIONS 3
#define MENSEC_HEADER 0
#define MENSEC_TIMERS 1
#define MENSEC_FOOTER 2

#define MENROWS_HEADER 3
#define MENROWS_FOOTER 3

#define MENROW_HEADER_START_ALL 0
#define MENROW_HEADER_PAUSE_ALL 1
#define MENROW_HEADER_RESET_ALL 2

#define MENROW_FOOTER_ADD_TIMER 0
#define MENROW_FOOTER_ADD_STOPWATCH 1
#define MENROW_FOOTER_CLEAR 2

#define MENU_ICON_OK 0
#define MENU_ICON_PROBLEM 1
#define MENU_ICON_UNKNOWN 2

#define ICON_TIMER_STOPPED 0
#define ICON_TIMER_PAUSED 1
#define ICON_TIMER_RUNNING 2
#define ICON_TIMER_FINISHED 3

#define COOKIE_TIMER 1

void     handle_init(AppContextRef ctx);
void     window_load(Window *me);
void     window_unload(Window *me);
void     window_appear(Window* me);
void     load_bitmaps();
void     unload_bitmaps();
void     set_timer(AppContextRef ctx);
void     handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie);
uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
int16_t  menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
void     menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void     menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
void     menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
void     menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void     menu_draw_header_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void     menu_draw_footer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void     menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void     jump_to_timer(int t, bool animate);
void     show_add_timer();

#if ROCKSHOT
void     http_success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx);
#endif

Window         window;
Window         window_add_timer;
MenuLayer      layer_menu;
AppContextRef  app_ctx;
AppTimerHandle timer_handle;
HeapBitmap     timer_icons[4];

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .timer_handler = &handle_timer
  };

  #if ROCKSHOT
  handlers.messaging_info = (PebbleAppMessagingInfo) {
    .buffer_sizes = {
      .inbound = 124,
      .outbound = 124,
    },
  };
  http_capture_main(&handlers);
  #endif

  app_event_loop(params, &handlers);
}

void handle_init(AppContextRef ctx) {
  app_ctx = ctx;

  resource_init_current_app(&APP_RESOURCES);

  #if ROCKSHOT
  http_set_app_id(15);
  http_register_callbacks((HTTPCallbacks) {
    .success = http_success
  }, NULL);
  http_capture_init(ctx);
  #endif

  window_init(&window, "Multi Timer Main Window");
  window_stack_push(&window, true);
  window_set_window_handlers(&window, (WindowHandlers){
    .load = window_load,
    .appear = window_appear,
    .unload = window_unload,
  });

  init_add_window(&window_add_timer, ctx);
  init_vibe_window(ctx);
}

void window_load(Window *me) {
  load_bitmaps();
  menu_layer_init(&layer_menu, me->layer.bounds);
  menu_layer_set_callbacks(&layer_menu, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
    .select_long_click = menu_select_long_click_callback,
  });
  menu_layer_set_click_config_onto_window(&layer_menu, me);
  layer_add_child(&me->layer, menu_layer_get_layer(&layer_menu));
  jump_to_timer(0, false);
  set_timer(app_ctx);
}

void window_unload(Window *me) {
  unload_bitmaps();
}

void window_appear(Window* me) {
  menu_layer_reload_data(&layer_menu);
}

uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return MENU_SECTIONS;
}

uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENSEC_HEADER:
      return MENROWS_HEADER;
    break;
    case MENSEC_FOOTER:
      return MENROWS_FOOTER;
    break;
    case MENSEC_TIMERS: {
      if (get_timer_count() == 0) {
        return 1;
      }
      return get_timer_count();
    }
    break;
  }
  return 0;
}

int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case MENSEC_HEADER:
      menu_cell_basic_header_draw(ctx, cell_layer, "Timer Controls");
    break;
    case MENSEC_FOOTER:
      menu_cell_basic_header_draw(ctx, cell_layer, "Options");
    break;
    case MENSEC_TIMERS: {
      char timer_header[20];
      snprintf(timer_header, sizeof(timer_header), "Timers [%d]", get_timer_count());
      menu_cell_basic_header_draw(ctx, cell_layer, "Timers");
    }
    break;
  }
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case MENSEC_HEADER:
      menu_draw_header_row(ctx, cell_layer, cell_index, data);
    break;
    case MENSEC_FOOTER:
      menu_draw_footer_row(ctx, cell_layer, cell_index, data);
    break;
    case MENSEC_TIMERS:
      menu_draw_timer_row(ctx, cell_layer, cell_index, data);
    break;
  }
}

void menu_draw_header_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENROW_HEADER_START_ALL:
      menu_cell_basic_draw(ctx, cell_layer, "Start All", "Start all non-running timers.", NULL);
    break;
    case MENROW_HEADER_PAUSE_ALL:
      menu_cell_basic_draw(ctx, cell_layer, "Pause All", "Pause all running timers.", NULL);
    break;
    case MENROW_HEADER_RESET_ALL:
      menu_cell_basic_draw(ctx, cell_layer, "Reset All", "Reset all timers.", NULL);
    break;
  }
}

void menu_draw_footer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENROW_FOOTER_ADD_TIMER:
      menu_cell_basic_draw(ctx, cell_layer, "Add Timer", "Adds a new timer.", NULL);
    break;
    case MENROW_FOOTER_ADD_STOPWATCH:
      menu_cell_basic_draw(ctx, cell_layer, "Add Stopwatch", "Adds a stopwatch.", NULL);
    break;
    case MENROW_FOOTER_CLEAR:
      menu_cell_basic_draw(ctx, cell_layer, "Clear Timers", "Removes all timers.", NULL);
    break;
  }
}

void menu_draw_timer_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  if (get_timer_count() == 0 && cell_index->row == 0) {
    menu_cell_basic_draw(ctx, cell_layer, "No Timers", "Click to add one.", NULL);
  }
  else {
    Timer* timer = get_timer(cell_index->row);
    if (! timer) {
      return;
    }
    char time_left[10];
    snprintf(time_left, sizeof(time_left), "%02d:%02d", timer->time_left / 60, timer->time_left % 60);
    GBitmap* row_bmp = NULL;
    char sub_message[50];
    switch (timer->status) {
      case TIMER_RUNNING:
        row_bmp = &timer_icons[ICON_TIMER_RUNNING].bmp;
        strcpy(sub_message, "Pause");
      break;
      case TIMER_PAUSED:
        row_bmp = &timer_icons[ICON_TIMER_PAUSED].bmp;
        strcpy(sub_message, "Resume // Reset");
      break;
      case TIMER_STOPPED:
        row_bmp = &timer_icons[ICON_TIMER_STOPPED].bmp;
        strcpy(sub_message, "Start // Delete");
      break;
      case TIMER_FINISHED:
        row_bmp = &timer_icons[ICON_TIMER_FINISHED].bmp;
        strcpy(sub_message, "Reset // Delete");
      break;
    }
    menu_cell_basic_draw(ctx, cell_layer, time_left, sub_message, row_bmp);
  }
}

void set_timer(AppContextRef ctx) {
  timer_handle = app_timer_send_event(ctx, 1000, COOKIE_TIMER);
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  switch (cookie) {
    case COOKIE_TIMER: {
      for (int t = 0; t < get_timer_count(); t += 1) {
        timer_tick(get_timer(t));
      }
      set_timer(ctx);
      menu_layer_reload_data(&layer_menu);
    }
    break;
    case COOKIE_AW_FLICKER: {
      add_window_timer();
    }
    break;
    case COOKIE_VW_VIBRATE: {
      vibe_window_timer();
    }
    break;
  }
}

void load_bitmaps() {
  heap_bitmap_init(&timer_icons[ICON_TIMER_RUNNING], RESOURCE_ID_ICON_TIMER_RUNNING);
  heap_bitmap_init(&timer_icons[ICON_TIMER_PAUSED], RESOURCE_ID_ICON_TIMER_PAUSED);
  heap_bitmap_init(&timer_icons[ICON_TIMER_STOPPED], RESOURCE_ID_ICON_TIMER_STOPPED);
  heap_bitmap_init(&timer_icons[ICON_TIMER_FINISHED], RESOURCE_ID_ICON_TIMER_FINISHED);
}

void unload_bitmaps() {
  heap_bitmap_deinit(&timer_icons[ICON_TIMER_RUNNING]);
  heap_bitmap_deinit(&timer_icons[ICON_TIMER_PAUSED]);
  heap_bitmap_deinit(&timer_icons[ICON_TIMER_STOPPED]);
  heap_bitmap_deinit(&timer_icons[ICON_TIMER_FINISHED]);
}

void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->section) {
    case MENSEC_HEADER: {
      switch (cell_index->row) {
        case MENROW_HEADER_START_ALL: {
          for (int t = 0; t < get_timer_count(); t += 1) {
            timer_resume(get_timer(t));
          }
          jump_to_timer(0, true);
        }
        break;
        case MENROW_HEADER_PAUSE_ALL: {
          for (int t = 0; t < get_timer_count(); t += 1) {
            if (get_timer(t)->status == TIMER_RUNNING) {
              timer_pause(get_timer(t));
            }
          }
          jump_to_timer(0, true);
        }
        break;
        case MENROW_HEADER_RESET_ALL: {
          for (int t = 0; t < get_timer_count(); t += 1) {
            timer_reset(get_timer(t));
          }
          jump_to_timer(0, true);
        }
        break;
      }
    }
    break;
    case MENSEC_FOOTER: {
      switch (cell_index->row) {
        case MENROW_FOOTER_ADD_TIMER:
          show_add_timer();
          return;
        break;
        case MENROW_FOOTER_ADD_STOPWATCH: {
          add_stopwatch();
          menu_layer_reload_data(&layer_menu);
          jump_to_timer(get_timer_count() - 1, true);
        }
        break;
        case MENROW_FOOTER_CLEAR:
         clear_timers();
         jump_to_timer(0, true);
        break;
      }
    }
    break;
    case MENSEC_TIMERS: {
      if (get_timer_count() == 0) {
        show_add_timer();
        return;
      }
      Timer* timer = get_timer(cell_index->row);
      if (! timer) {
        return;
      }
      switch (timer->status) {
        case TIMER_RUNNING:
          timer_pause(timer);
        break;
        case TIMER_PAUSED:
          timer_resume(timer);
        break;
        case TIMER_STOPPED:
          timer_start(timer);
        break;
        case TIMER_FINISHED:
          timer_reset(timer);
        break;
      }
    }
    break;
  }
  menu_layer_reload_data(&layer_menu);
}

void menu_select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if (cell_index->section != MENSEC_TIMERS) {
    return;
  }
  Timer* timer = get_timer(cell_index->row);
  if (! timer) {
    return;
  }
  switch (timer->status) {
    case TIMER_PAUSED:
      timer_reset(timer);
    break;
    case TIMER_STOPPED:
    case TIMER_FINISHED:
      remove_timer(cell_index->row);
    break;
    default:
      return;
  }
  menu_layer_reload_data(&layer_menu);
}

void show_add_timer() {
  show_add_window(&window_add_timer);
}

void jump_to_timer(int t, bool animate) {
  MenuIndex index =  { MENSEC_TIMERS, t };
  menu_layer_set_selected_index(&layer_menu, index, MenuRowAlignCenter, animate);
}

#if ROCKSHOT
void http_success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {

}
#endif