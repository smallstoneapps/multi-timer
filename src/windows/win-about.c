#include <pebble.h>
#include <scroll-text-layer.h>
#include <pebble-assist.h>
#include "win-about.h"
#include "../generated/appinfo.h"

static void window_load(Window* window);
static void window_unload(Window* window);
static void layer_header_update(Layer* layer, GContext* ctx);

static Window* s_window;
static Layer* s_layer_header;
static ScrollTextLayer* s_layer_scroll;
char* s_text;

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

  s_text = malloc(448);
  resource_load(resource_get_handle(RESOURCE_ID_TEXT_ABOUT), (uint8_t*)s_text, 448);

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