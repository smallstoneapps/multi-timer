/***
 * Pebble Assist
 * Copyright (C) 2013 Matthew Tole
 ***/

#pragma once

#ifndef MENU_CELL_BASIC_CELL_HEIGHT
#define MENU_CELL_BASIC_CELL_HEIGHT 44
#endif

#ifndef PEBBLE_HEIGHT
#define PEBBLE_HEIGHT 168
#endif

#ifndef PEBBLE_WIDTH
#define PEBBLE_WIDTH 144
#endif

#ifndef STATUS_HEIGHT
#define STATUS_HEIGHT 16
#endif

#define layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), layer)
#define text_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), text_layer_get_layer(layer))
#define bitmap_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layer))
#define inverter_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(layer))
#define menu_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), menu_layer_get_layer(layer))
#define simple_menu_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(layer))
#define text_layer_set_system_font(layer, font) text_layer_set_font(layer, fonts_get_system_font(font))
#define text_layer_set_colours(layer, foreground, background) text_layer_set_text_color(layer, foreground); text_layer_set_background_color(layer, background)
#define text_layer_set_colors(layer, foreground, background) text_layer_set_text_color(layer, foreground); text_layer_set_background_color(layer, background)
#define layer_create_fullscreen(window) layer_create(layer_get_bounds(window_get_root_layer(window)));
#define text_layer_create_fullscreen(window) text_layer_create(layer_get_bounds(window_get_root_layer(window)));
#define menu_layer_create_fullscreen(window) menu_layer_create(layer_get_bounds(window_get_root_layer(window)));
#define action_bar_layer_create_and_add(action_bar, window) action_bar = action_bar_layer_create(); action_bar_layer_add_to_window(action_bar, window)

#define window_destroy_safe(window) if (window != NULL) { window_destroy(window); }
#define number_window_destroy_safe(window) if (window != NULL ) { number_window_destroy(window); }
#define text_layer_destroy_safe(layer) if (layer != NULL) { text_layer_destroy(layer); }
#define bitmap_layer_destroy_safe(layer) if (layer != NULL) { bitmap_layer_destroy(layer); }
#define layer_destroy_safe(layer) if (layer != NULL) { layer_destroy(layer); }
#define menu_layer_destroy_safe(layer) if (layer != NULL) { menu_layer_destroy(layer); }
#define simple_menu_layer_destroy_safe(layer) if (layer != NULL) { simple_menu_layer_destroy(layer); }
#define action_bar_layer_destroy_safe(layer) if (layer != NULL) { action_bar_layer_destroy(layer); }
#define app_timer_cancel_safe(timer) if (timer != NULL) { app_timer_cancel(timer); timer = NULL; }

#define persist_read_int_safe(key, value) if (persist_exists(key)) { return persist_read_int(key); } else { return value; }

#define fonts_load_resource_font(resource) fonts_load_custom_font(resource_get_handle(resource))

#define action_bar_layer_clear_icons(action_bar) action_bar_layer_clear_icon(action_bar, BUTTON_ID_SELECT); action_bar_layer_clear_icon(action_bar, BUTTON_ID_DOWN); action_bar_layer_clear_icon(action_bar, BUTTON_ID_UP)
