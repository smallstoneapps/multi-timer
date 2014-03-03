/***
 * UK Transport
 * Copyright (C) 2013 Matthew Tole
 *
 * windows/about.c
 ***/

#include <pebble.h>
#include "win-about.h"

static void window_load(Window* window);
static void window_unload(Window* window);

static Window* window;
static ScrollLayer* layer_scroll;
static TextLayer* layer_text;
static const int vert_scroll_text_padding = 4;
static const char* text_about = "Multi Timer is a Pebble app developed by Matthew Tole.\n\nIf you like this app, please consider donating to help fund future development.\n\nGo to http://matthewtole.com/pebble/ for details.";

void win_about_create(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
}

void win_about_destroy(void) {
  window_destroy(window);
}

void win_about_show(bool animated) {
  window_stack_push(window, animated);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect max_text_bounds = GRect(4, -2, bounds.size.w - 8, 2000);
  layer_scroll = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(layer_scroll, window);
  layer_text = text_layer_create(max_text_bounds);
  text_layer_set_text(layer_text, text_about);
  text_layer_set_font(layer_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  GSize max_size = text_layer_get_content_size(layer_text);
  text_layer_set_size(layer_text, max_size);
  scroll_layer_set_content_size(layer_scroll, GSize(bounds.size.w, max_size.h + vert_scroll_text_padding));
  scroll_layer_add_child(layer_scroll, text_layer_get_layer(layer_text));
  layer_add_child(window_layer, scroll_layer_get_layer(layer_scroll));
}

static void window_unload(Window* window) {
  text_layer_destroy(layer_text);
  scroll_layer_destroy(layer_scroll);
}