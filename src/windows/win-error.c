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

src/windows/win-error.c

*/

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