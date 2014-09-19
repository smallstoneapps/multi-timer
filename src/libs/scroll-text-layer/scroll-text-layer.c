/*

Scroll Text Layer v1.0
A Pebble library for having a scrollable text layer in your Pebble apps.
http://smallstoneapps.github.io/scroll-text-layer/

----------------------

The MIT License (MIT)

Copyright © 2013-2014 Matthew Tole

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

src/scroll-text-layer.c

*/

#include <pebble.h>
#include "scroll-text-layer.h"

#define MAX_HEIGHT 2000
#define PADDING_X 4
#define PADDING_Y 4

struct ScrollTextLayer {
  TextLayer* text_layer;
  ScrollLayer* scroll_layer;
};

static void scroll_text_layer_update(ScrollTextLayer* layer);

ScrollTextLayer* scroll_text_layer_create(GRect rect) {
  ScrollTextLayer* stl = malloc(sizeof(ScrollTextLayer));
  stl->scroll_layer = scroll_layer_create(rect);
  GRect max_text_bounds = GRect(PADDING_X, PADDING_Y, rect.size.w - (PADDING_X * 2), MAX_HEIGHT);
  stl->text_layer = text_layer_create(max_text_bounds);
  scroll_layer_add_child(stl->scroll_layer, text_layer_get_layer(stl->text_layer));
  return stl;
}

void scroll_text_layer_destroy(ScrollTextLayer* layer) {
  if (NULL == layer) {
    return;
  }
  text_layer_destroy(scroll_text_layer_get_text_layer(layer));
  scroll_layer_destroy(scroll_text_layer_get_scroll_layer(layer));
  free(layer);
}

void scroll_text_layer_add_to_window(ScrollTextLayer* layer, Window* window) {
  if (NULL == layer || NULL == window) {
    return;
  }
  scroll_layer_set_click_config_onto_window(layer->scroll_layer, window);
  layer_add_child(window_get_root_layer(window), scroll_layer_get_layer(layer->scroll_layer));
}

void scroll_text_layer_set_text(ScrollTextLayer* layer, char* text) {
  if (NULL == layer) {
    return;
  }
  text_layer_set_text(layer->text_layer, text);
  scroll_text_layer_update(layer);
}

TextLayer* scroll_text_layer_get_text_layer(ScrollTextLayer* layer) {
  if (NULL == layer) {
    return NULL;
  }
  return layer->text_layer;
}

ScrollLayer* scroll_text_layer_get_scroll_layer(ScrollTextLayer* layer) {
  if (NULL == layer) {
    return NULL;
  }
  return layer->scroll_layer;
}

void scroll_text_layer_set_font(ScrollTextLayer* layer, GFont* font) {
  text_layer_set_font(scroll_text_layer_get_text_layer(layer), font);
  scroll_text_layer_update(layer);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void scroll_text_layer_update(ScrollTextLayer* layer) {
  GSize max_size = text_layer_get_content_size(layer->text_layer);
  text_layer_set_size(layer->text_layer, max_size);
  GRect bounds = layer_get_bounds(scroll_layer_get_layer(layer->scroll_layer));
  scroll_layer_set_content_size(layer->scroll_layer, GSize(bounds.size.w, max_size.h + (PADDING_Y * 3)));
}