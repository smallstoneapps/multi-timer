/*

Multi Timer v2.7.1
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

src/windows/win-about.c

*/

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
  layer_destroy_safe(layer_header);
}

static void layer_header_update(Layer* layer, GContext* ctx) {
  char str[32];
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  snprintf(str, 32, "Multi Timer %s", VERSION_LABEL);
  graphics_draw_text(ctx, str, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 0, 144, 22), 0, GTextAlignmentCenter, NULL);
}