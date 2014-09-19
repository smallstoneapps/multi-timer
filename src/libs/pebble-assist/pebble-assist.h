/***
 * Pebble Assist
 * Copyright (C) 2014 Matthew Tole
 *
 * Version 0.2.3
 ***/

/**
The MIT License (MIT)

Copyright (c) 2013 Matthew Tole

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
**/

#pragma once

// Pointer Logging
static const bool SHOW_POINTERS = false;

// Missing Constants

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

// Shorthand App Logging

#define DISABLE_LOGGING false

#if DISABLE_LOGGING
#define LOG(...)
#define DEBUG(...)
#define INFO(...)
#define WARN(...)
#define ERROR(...)
#else
#define LOG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) app_log(APP_LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) app_log(APP_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#endif

// Window Helpers
#define window_destroy_safe(window) if (NULL != window) { if (SHOW_POINTERS) { DEBUG("Window Destroy Safe: %p", window); } window_destroy(window); window = NULL; }

// Number Window Helpers
#define number_window_destroy_safe(window) if (NULL != window) { if (SHOW_POINTERS) { DEBUG("Number Window Destroy Safe: %p", window); } number_window_destroy(window); window = NULL; }

// Layer Helpers
#define layer_create_fullscreen(window) layer_create(layer_get_bounds(window_get_root_layer(window)));
#define layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), layer)
#define layer_show(layer) layer_set_hidden(layer, false)
#define layer_hide(layer) layer_set_hidden(layer, true)
#define layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Layer Destroy Safe: %p", layer); } layer_destroy(layer); layer = NULL; }

// Scroll Layer Helpers
#define scroll_layer_create_fullscreeen(window) scroll_layer_create(layer_get_bounds(window_get_root_layer(window)))
#define scroll_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), scroll_layer_get_layer(layer))
#define scroll_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Scroll Layer Destroy Safe: %p", layer); } scroll_layer_destroy(layer); layer = NULL; }

// Text Layer Helpers
#define text_layer_create_fullscreen(window) text_layer_create(layer_get_bounds(window_get_root_layer(window)));
#define text_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), text_layer_get_layer(layer))
#define text_layer_set_system_font(layer, font) text_layer_set_font(layer, fonts_get_system_font(font))
#define text_layer_set_colours(layer, foreground, background) text_layer_set_text_color(layer, foreground); text_layer_set_background_color(layer, background)
#define text_layer_set_colors(layer, foreground, background) text_layer_set_text_color(layer, foreground); text_layer_set_background_color(layer, background)
#define text_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Text Layer Destroy Safe: %p", layer); } text_layer_destroy(layer); layer = NULL; }

// Bitmap Layer Helpers
#define bitmap_layer_create_fullscreen(window) bitmap_layer_create(layer_get_bounds(window_get_root_layer(window)));
#define bitmap_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layer))
#define bitmap_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Bitmap Layer Destroy Safe: %p", layer); } bitmap_layer_destroy(layer); layer = NULL; }

// GBitmap
#define gbitmap_destroy_safe(gbitmap) if (NULL != gbitmap) {if (SHOW_POINTERS) { DEBUG("GBitmap Destroy Safe: %p", gbitmap); } gbitmap_destroy(gbitmap); gbitmap = NULL; }

// Inverter Layer Helpers
#define inverter_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(layer))
#define inverter_layer_create_fullscreen(layer, window) inverter_layer_create(layer_get_bounds(window_get_root_layer(window)))
#define inverter_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Inverter Layer Destroy Safe: %p", layer); } inverter_layer_destroy(layer); layer = NULL; }

// Menu Layer Helpers
#define menu_layer_create_fullscreen(window) menu_layer_create(layer_get_bounds(window_get_root_layer(window)))
#define menu_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), menu_layer_get_layer(layer)); menu_layer_set_click_config_onto_window(layer, window)
#define menu_layer_reload_data_and_mark_dirty(layer) menu_layer_reload_data(layer); layer_mark_dirty(menu_layer_get_layer(layer));
#define menu_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Menu Layer Destroy Safe: %p", layer); } menu_layer_destroy(layer); layer = NULL; }

// Simple Menu Layer Helpers
#define simple_menu_layer_create_fullscreen(window, sections, num_sections, callback_context) simple_menu_layer_create(layer_get_bounds(window_get_root_layer(window)), sections, num_sections, callback_context)
#define simple_menu_layer_add_to_window(layer, window) layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(layer))
#define simple_menu_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Simple Menu Layer Destroy Safe: %p", layer); } simple_menu_layer_destroy(layer); layer = NULL; }

// Action Bar Layer Helpers
#define action_bar_layer_create_in_window(action_bar, window) action_bar = action_bar_layer_create(); action_bar_layer_add_to_window(action_bar, window)
#define action_bar_layer_clear_icons(action_bar) action_bar_layer_clear_icon(action_bar, BUTTON_ID_SELECT); action_bar_layer_clear_icon(action_bar, BUTTON_ID_DOWN); action_bar_layer_clear_icon(action_bar, BUTTON_ID_UP)
#define action_bar_layer_destroy_safe(layer) if (NULL != layer) { if (SHOW_POINTERS) { DEBUG("Action Bar Layer Destroy Safe: %p", layer); } action_bar_layer_destroy(layer); layer = NULL; }

// Dynamic Memory Helpers
#define free_safe(ptr) if (NULL != ptr) { if (SHOW_POINTERS) { DEBUG("Free Safe: %p", ptr); } free(ptr); ptr = NULL; }

// App Timer Helpers
#define app_timer_cancel_safe(timer) if (timer != NULL) { app_timer_cancel(timer); timer = NULL; }

// Font Helpers
#define fonts_load_resource_font(resource) fonts_load_custom_font(resource_get_handle(resource))

// App Message Helpers
#define app_message_open_max() app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum())

// Persistence Helpers
#define persist_read_int_safe(key, value) (persist_exists(key) ? persist_read_int(key) : value);
#define persist_read_bool_safe(key, value) (persist_exists(key) ? persist_read_bool(key) : value);
