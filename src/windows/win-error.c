/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * windows/win-error.c
 ***/

#include <pebble.h>
#include "win-error.h"
#include "../libs/scroll-text-layer/scroll-text-layer.h"
#include "../libs/pebble-assist/pebble-assist.h"

static void window_load(Window* window);
static void window_unload(Window* window);
static void click_config_provider(void* context);
static void click_select(ClickRecognizerRef recognizer, void *context);

static Window* window;
static ScrollTextLayer* layer;
static char* message = NULL;

void win_error_init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
}

void win_error_set_text(char* text) {
  if (NULL != message) {
    free(message);
  }
  message = malloc(sizeof(char) * strlen(text));
  strncpy(message, text, strlen(text));
  scroll_text_layer_set_text(layer, message);
}

void win_error_show(void) {
  window_stack_push(window, true);
}

void win_error_destroy() {
  if (NULL != message) {
    free(message);
  }
  window_destroy(window);
}

//

static void window_load(Window* window) {
  layer = scroll_text_layer_create_fullscreen(window);
  scroll_text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  scroll_text_layer_add_to_window(layer, window);
  win_error_set_text(message);
  scroll_layer_set_callbacks(scroll_text_layer_get_scroll_layer(layer), (ScrollLayerCallbacks) {
    .click_config_provider = click_config_provider
  });
}

static void window_unload(Window* window) {
  scroll_text_layer_destroy(layer);
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, click_select);
}

static void click_select(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}