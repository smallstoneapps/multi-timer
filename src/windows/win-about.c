/***
 * UK Transport
 * Copyright (C) 2013 - 2014 Matthew Tole
 *
 * windows/about.c
 ***/

#include <pebble.h>
#include "win-about.h"
#include "../libs/scroll-text-layer/scroll-text-layer.h"
#include "../libs/pebble-assist/pebble-assist.h"
#include "../generated/appinfo.h"

static void window_load(Window* window);
static void window_unload(Window* window);
static void layer_header_update(Layer* layer, GContext* ctx);

static Window* window;
static ScrollTextLayer* layer;
static Layer* layer_header;
static char* text_about = "Multi Timer is a Pebble app developed by Matthew Tole.\n\nIf you like this app, please consider donating to help fund future development.\n\nGo to http://matthewtole.com/pebble/ for details.";

void win_about_init(void) {
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
  layer = scroll_text_layer_create(GRect(0, 26, 144, 130));
  scroll_text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  scroll_text_layer_set_text(layer, text_about);
  scroll_text_layer_add_to_window(layer, window);

  layer_header = layer_create(GRect(0, 0, 144, 26));
  layer_set_update_proc(layer_header, layer_header_update);
  layer_add_to_window(layer_header, window);
}

static void window_unload(Window* window) {
  scroll_text_layer_destroy(layer);
}

static void layer_header_update(Layer* layer, GContext* ctx) {
  char str[32];
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  snprintf(str, 32, "Multi Timer %s", VERSION_LABEL);
  graphics_draw_text(ctx, str, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 0, 144, 22), 0, GTextAlignmentCenter, NULL);
}