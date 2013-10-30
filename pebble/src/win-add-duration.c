/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * win-add-duration.c
 ***/

#include <pebble_os.h>
#include <pebble_fonts.h>
#include <pebble_app.h>

#include "win-add-duration.h"
#include "timer.h"

#define MODE_MINUTES 0
#define MODE_SECONDS 1

static void layer_update(Layer* me, GContext* ctx);
static void Layer_action_bar_click_config_provider(ClickConfig **config, void *context);
static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context);
static void blink_timer_callback(void* data);
static void blink_timer_start(void);
static void blink_timer_cancel(void);

static Window* window;
static Layer* layer;
static ActionBarLayer* layer_action_bar;
static Timer* timer = NULL;
static AppTimer* blink_timer = NULL;
static int mode = MODE_MINUTES;
static bool blink = false;
static GFont font_duration;

void win_add_duration_init(void) {
  window = window_create();

  layer = layer_create(layer_get_frame(window_get_root_layer(window)));
  layer_set_update_proc(layer, layer_update);
  layer_add_child(window_get_root_layer(window), layer);

  layer_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(layer_action_bar, window);
  action_bar_layer_set_click_config_provider(layer_action_bar, Layer_action_bar_click_config_provider);

  font_duration = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AUDI_40_BOLD));
}

void win_add_duration_show(Timer* tmr) {
  timer = tmr;
  window_stack_push(window, true);
  mode = MODE_MINUTES;
  blink_timer_start();
}

void win_add_duration_destroy(void) {
  action_bar_layer_destroy(layer_action_bar);
  layer_destroy(layer);
  window_destroy(window);
  fonts_unload_custom_font(font_duration);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void layer_update(Layer* me, GContext* ctx) {
  if (timer == NULL) {
    return;
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  // Draw Minutes
  char* min_str = "000";
  snprintf(min_str, 3, "%02d", timer->length / 60);
  if (mode != MODE_MINUTES || ! blink) {
    graphics_text_draw(ctx, min_str, font_duration,
      GRect(0, 50, 54, 40), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
  }

  // Draw Seconds
  char* sec_str = "000";
  snprintf(sec_str, 3, "%02d", timer->length % 60);
  if (mode != MODE_SECONDS || ! blink) {
    graphics_text_draw(ctx, sec_str, font_duration,
      GRect(70, 50, 50, 40), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  }

  // Draw Separator
  graphics_text_draw(ctx, ":", font_duration,
    GRect(47, 47, 30, 40), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

}

static void Layer_action_bar_click_config_provider(ClickConfig **config, void *context) {
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) action_bar_layer_down_handler;
  config[BUTTON_ID_UP]->click.handler = (ClickHandler)  action_bar_layer_up_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler)  action_bar_layer_select_handler;
}

static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_MINUTES:
      if (timer->length >= 60) {
        timer->length -= 60;
        blink_timer_start();
      }
    break;
    case MODE_SECONDS:
      if (timer->length > 0) {
        timer->length -= 1;
        blink_timer_start();
      }
    break;
  }
}

static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_MINUTES:
      timer->length += 60;
      blink_timer_start();
    break;
    case MODE_SECONDS:
      timer->length += 1;
      blink_timer_start();
    break;
  }
}

static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context) {
  switch (mode) {
    case MODE_MINUTES:
      mode = MODE_SECONDS;
    break;
    case MODE_SECONDS:
      mode = MODE_MINUTES;
    break;
  }
}

static void blink_timer_callback(void* data) {
  blink = ! blink;
  blink_timer_start();
}

static void blink_timer_start(void) {
  blink_timer_cancel();
  blink_timer = app_timer_register(500, blink_timer_callback, NULL);
  layer_mark_dirty(layer);
}

static void blink_timer_cancel(void) {
  if (blink_timer != NULL) {
    app_timer_cancel(blink_timer);
    blink_timer = NULL;
  }
}
