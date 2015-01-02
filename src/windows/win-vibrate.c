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

src/windows/win-vibrate.c

*/

#include <pebble.h>

#include "win-vibrate.h"
#include <bitmap-loader.h>
#include <pebble-assist.h>
#include "../common.h"

void click_config_provider(Window* window);
void click_handler(ClickRecognizerRef recognizer, void* context);
void window_appear(Window* window);
void window_disappear(Window* window);
void do_vibrate(void);
void vibe_timer_callback(void* data);

Window* window;
BitmapLayer* bmp_layer;
AppTimer* vibe_timer = NULL;
bool is_visible = false;

void win_vibrate_init(void) {
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_set_click_config_provider(window, (ClickConfigProvider)click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .appear = window_appear,
    .disappear = window_disappear
  });

  bmp_layer = bitmap_layer_create(GRect(40, 44, 64, 64));
  bitmap_layer_set_compositing_mode(bmp_layer, GCompOpAssignInverted);
  bitmap_layer_set_bitmap(bmp_layer, bitmaps_get_bitmap(RESOURCE_ID_IMAGE_ALARM));
  bitmap_layer_add_to_window(bmp_layer, window);
}

void win_vibrate_show(void) {
  if (window_stack_contains_window(window)) {
    return;
  }
  window_stack_push(window, false);
}

void win_vibrate_destroy(void) {
  bitmap_layer_destroy(bmp_layer);
  window_destroy(window);
}

bool win_vibrate_is_visible(void) {
  return is_visible;
}

//----------------------------------------------------------------------------//

void click_config_provider(Window* window) {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, click_handler);
}

void click_handler(ClickRecognizerRef recognizer, void* context) {
  window_stack_pop(true);
}

void window_appear(Window* window) {
  do_vibrate();
  is_visible = true;
}

void window_disappear(Window* window) {
  app_timer_cancel_safe(vibe_timer);
  is_visible = false;
}

void do_vibrate(void) {
  vibes_long_pulse();
  vibe_timer = app_timer_register(1000, vibe_timer_callback, NULL);
}

void vibe_timer_callback(void* data) {
  vibe_timer = NULL;
  do_vibrate();
}