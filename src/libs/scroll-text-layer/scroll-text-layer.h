/***
 * Scroll Text Layer
 * Copyright © 2013-2014 Matthew Tole
 * MIT License
 ***/

#include <pebble.h>

struct ScrollTextLayer;
typedef struct ScrollTextLayer ScrollTextLayer;

ScrollTextLayer* scroll_text_layer_create(GRect rect);
void scroll_text_layer_destroy(ScrollTextLayer* layer);
TextLayer* scroll_text_layer_get_text_layer(ScrollTextLayer* layer);
ScrollLayer* scroll_text_layer_get_scroll_layer(ScrollTextLayer* layer);
void scroll_text_layer_add_to_window(ScrollTextLayer* layer, Window* window);
void scroll_text_layer_set_text(ScrollTextLayer* layer, char* text);

#define scroll_text_layer_create_fullscreen(window) scroll_text_layer_create(layer_get_bounds(window_get_root_layer(window)));
#define scroll_text_layer_set_text_color(layer, color) text_layer_set_text_color(scroll_text_layer_get_text_layer(layer), color)
#define scroll_text_layer_set_background_color(layer, color) text_layer_set_background_color(scroll_text_layer_get_text_layer(layer), color)
#define scroll_text_layer_set_font(layer, font) text_layer_set_font(scroll_text_layer_get_text_layer(layer), font)
#define scroll_text_layer_set_text_alignment(layer, alignment) text_layer_set_text_alignment(scroll_text_layer_get_text_layer(layer), alignment)
#define scroll_text_layer_set_system_font(layer, font) scroll_text_layer_get_text_layer(layer, fonts_get_system_font(font))