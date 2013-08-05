#include "pebble_fonts.h"
#include "pebble_app.h"
#include "vibe-window.h"

void vibe_window_load(Window* me);
void vibe_window_unload(Window* me);
void vibe_window_appear(Window* me);
void vibe_window_disappear(Window* me);
void vw_click_config_provider(ClickConfig **config, Window *window);
void vw_clicked(ClickRecognizerRef recognizer, Window* window);
void vw_set_tick(AppContextRef ctx);
void vw_cancel_tick(AppContextRef ctx);
void do_vibrate();

Window vibe_window;
TextLayer vw_tl_message;
AppTimerHandle vw_timer_handle;
AppContextRef vw_app_ctx;
bool vw_loaded = false;
bool vw_showing = false;

void init_vibe_window(AppContextRef ctx) {
  window_init(&vibe_window, "Multi Timer Vibration Window");
  window_set_window_handlers(&vibe_window, (WindowHandlers){
    .load = vibe_window_load,
    .unload = vibe_window_unload,
    .appear = vibe_window_appear,
    .disappear = vibe_window_disappear,
  });

  vw_app_ctx = ctx;
}

void show_vibe_window() {
  if (vw_showing) {
    return;
  }
  vw_showing = true;
  window_stack_push(&vibe_window, true);
  vw_set_tick(vw_app_ctx);
  do_vibrate();
}

void vibe_window_load(Window* me) {

  static char* message = "TIMER\nCOMPLETE";

  if (vw_loaded) {
    return;
  }
  vw_loaded = true;

  text_layer_init(&vw_tl_message, GRect(8, 44, 128, 56));
  text_layer_set_text_color(&vw_tl_message, GColorBlack);
  text_layer_set_background_color(&vw_tl_message, GColorClear);
  text_layer_set_font(&vw_tl_message, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&vw_tl_message, GTextAlignmentCenter);
  text_layer_set_overflow_mode(&vw_tl_message, GTextOverflowModeWordWrap);
  text_layer_set_text(&vw_tl_message, message);
  layer_add_child(&me->layer, &vw_tl_message.layer);

  window_set_click_config_provider(me, (ClickConfigProvider) vw_click_config_provider);
}

void vibe_window_unload(Window* me) {
  vw_cancel_tick(vw_app_ctx);
  vw_showing = false;
}

void vibe_window_appear(Window* me) {
}

void vibe_window_disappear(Window* me) {
}

void vw_click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) vw_clicked;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) vw_clicked;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) vw_clicked;
}


void vw_set_tick(AppContextRef ctx) {
  vw_timer_handle = app_timer_send_event(ctx, 1000, COOKIE_VW_VIBRATE);
}

void vw_cancel_tick(AppContextRef ctx) {
  app_timer_cancel_event(ctx, vw_timer_handle);
}

void vibe_window_timer() {
  do_vibrate();
  vw_set_tick(vw_app_ctx);
}

void vw_clicked(ClickRecognizerRef recognizer, Window* window) {
  window_stack_pop(true);
}

void do_vibrate() {
  const uint32_t seg[] = { 750 };
  VibePattern pattern = {
    .durations =  seg,
    .num_segments = ARRAY_LENGTH(seg)
  };
  vibes_enqueue_custom_pattern(pattern);
}