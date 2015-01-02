/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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
#include <scroll-text-layer.h>
#include <pebble-assist.h>
#include "win-about.h"
#include "../generated/appinfo.h"

#define ABOUT_LENGTH 187

static void window_load(Window* window);
static void window_unload(Window* window);
static void layer_header_update(Layer* layer, GContext* ctx);

static Window*          s_window;
static Layer*           s_layer_header;
static ScrollTextLayer* s_layer_scroll;
char*                   s_text;

void win_about_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void win_about_show(void) {
  window_stack_push(s_window, true);
}

static void window_load(Window* window) {
  s_layer_header = layer_create(GRect(0, 0, PEBBLE_WIDTH, 24));
  layer_set_update_proc(s_layer_header, layer_header_update);
  layer_add_to_window(s_layer_header, s_window);

  s_text = malloc(ABOUT_LENGTH);
  resource_load(resource_get_handle(RESOURCE_ID_TEXT_ABOUT), (uint8_t*)s_text, ABOUT_LENGTH);

  s_layer_scroll = scroll_text_layer_create(GRect(0, 24, PEBBLE_WIDTH, PEBBLE_HEIGHT - STATUS_HEIGHT - 24));
  scroll_text_layer_add_to_window(s_layer_scroll, s_window);
  scroll_text_layer_set_font(s_layer_scroll, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  scroll_text_layer_set_text(s_layer_scroll, s_text);
}

static void window_unload(Window* window) {
  layer_destroy(s_layer_header);
  scroll_text_layer_destroy(s_layer_scroll);
  free(s_text);
}

static void layer_header_update(Layer* layer, GContext* ctx) {
  char* version_string = malloc(24);
  snprintf(version_string, 24, "Multi Timer %s", VERSION_LABEL);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_draw_text(ctx, version_string, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(0, -5, PEBBLE_WIDTH, 24), GTextOverflowModeFill, GTextAlignmentCenter, 0);
  free(version_string);
}