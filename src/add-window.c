#include "pebble_fonts.h"
#include "pebble_app.h"
#include "add-window.h"
#include "timers.h"

#define AW_TL_HEIGHT 48

#define AW_MODE_MINUTES 0
#define AW_MODE_SECONDS 1
#define AW_MODE_VIBE 2
#define AW_MODE_SAVE 3

#define AW_MENU_ICON_NEXT 0
#define AW_MENU_ICON_PLUS 1
#define AW_MENU_ICON_MINUS 2
#define AW_MENU_ICON_SAVE 3
#define AW_MENU_ICON_TOGGLE 4

void add_window_load(Window* me);
void add_window_unload(Window* me);
void add_window_appear(Window* me);
void add_window_disappear(Window* me);
void aw_load_bitmaps();
void aw_unload_bitmaps();
void aw_update_actionbar_icons();
void aw_click_config_provider(ClickConfig **config, Window *window);
void aw_up_clicked(ClickRecognizerRef recognizer, Window* window);
void aw_down_clicked(ClickRecognizerRef recognizer, Window* window);
void aw_select_clicked(ClickRecognizerRef recognizer, Window* window);
void aw_up_held(ClickRecognizerRef recognizer, Window* window);
void aw_update_text();
void aw_set_tick();
void aw_cancel_tick();
void aw_create_timer();
void aw_reset();
void aw_fix_time();
void aw_fix_hidden();

TextLayer aw_tl_minutes;
TextLayer aw_tl_colon;
TextLayer aw_tl_seconds;
TextLayer aw_tl_vibrate;
TextLayer aw_tl_hint;
ActionBarLayer aw_actionbar;
AppTimerHandle aw_timer_handle;
AppContextRef aw_app_ctx;
HeapBitmap aw_menu_icons[5];
bool aw_loaded = false;

int minutes = 0;
int seconds = 0;
TimerVibration vibrate = TIMER_VIBE_OFF;
int mode = AW_MODE_MINUTES;

void init_add_window(Window* me, AppContextRef ctx) {
  window_init(me, "Multi Timer Add Timer Window");
  window_set_window_handlers(me, (WindowHandlers){
    .load = add_window_load,
    .unload = add_window_unload,
    .appear = add_window_appear,
    .disappear = add_window_disappear,
  });

  aw_app_ctx = ctx;
  action_bar_layer_init(&aw_actionbar);

}

void show_add_window(Window* me) {
  window_stack_push(me, true);
  aw_reset();
}

void add_window_load(Window* me) {

  aw_load_bitmaps();

  if (aw_loaded) {
    return;
  }
  aw_loaded = true;

  action_bar_layer_add_to_window(&aw_actionbar, me);

  text_layer_init(&aw_tl_minutes, GRect(0, 12, (144 - ACTION_BAR_WIDTH) / 2 - 6, AW_TL_HEIGHT));
  text_layer_set_text_color(&aw_tl_minutes, GColorBlack);
  text_layer_set_background_color(&aw_tl_minutes, GColorClear);
  text_layer_set_font(&aw_tl_minutes, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONO_BOLD_48)));
  text_layer_set_text_alignment(&aw_tl_minutes, GTextAlignmentRight);
  layer_add_child(&me->layer, &aw_tl_minutes.layer);

  text_layer_init(&aw_tl_colon, GRect((144 - ACTION_BAR_WIDTH) / 2 - 16, 10, 32, AW_TL_HEIGHT));
  text_layer_set_text_color(&aw_tl_colon, GColorBlack);
  text_layer_set_background_color(&aw_tl_colon, GColorClear);
  text_layer_set_font(&aw_tl_colon, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONO_BOLD_48)));
  text_layer_set_text_alignment(&aw_tl_colon, GTextAlignmentCenter);
  layer_add_child(&me->layer, &aw_tl_colon.layer);
  text_layer_set_text(&aw_tl_colon, ":");

  text_layer_init(&aw_tl_seconds, GRect((144 - ACTION_BAR_WIDTH) / 2 + 6, 12, (144 - ACTION_BAR_WIDTH) / 2 - 4, AW_TL_HEIGHT));
  text_layer_set_text_color(&aw_tl_seconds, GColorBlack);
  text_layer_set_background_color(&aw_tl_seconds, GColorClear);
  text_layer_set_font(&aw_tl_seconds, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONO_BOLD_48)));
  text_layer_set_text_alignment(&aw_tl_seconds, GTextAlignmentLeft);
  layer_add_child(&me->layer, &aw_tl_seconds.layer);

  text_layer_init(&aw_tl_vibrate, GRect(8, 20 + AW_TL_HEIGHT, 144 - ACTION_BAR_WIDTH - 16, 56));
  text_layer_set_text_color(&aw_tl_vibrate, GColorBlack);
  text_layer_set_background_color(&aw_tl_vibrate, GColorClear);
  text_layer_set_font(&aw_tl_vibrate, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&aw_tl_vibrate, GTextAlignmentCenter);
  layer_add_child(&me->layer, &aw_tl_vibrate.layer);

  text_layer_init(&aw_tl_hint, GRect(4, 111, 144 - ACTION_BAR_WIDTH - 8, 22));
  text_layer_set_text_color(&aw_tl_hint, GColorBlack);
  text_layer_set_background_color(&aw_tl_hint, GColorClear);
  text_layer_set_font(&aw_tl_hint, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&aw_tl_hint, GTextAlignmentRight);
  layer_add_child(&me->layer, &aw_tl_hint.layer);
}

void add_window_unload(Window* me) {
  aw_unload_bitmaps();
}

void add_window_appear(Window* me) {
  aw_update_text();
  aw_update_actionbar_icons();
  aw_set_tick(aw_app_ctx);
  aw_fix_hidden();
}

void add_window_disappear(Window* me) {
  aw_cancel_tick(aw_app_ctx);
}

void aw_load_bitmaps() {
  heap_bitmap_init(&aw_menu_icons[AW_MENU_ICON_NEXT], RESOURCE_ID_MENU_ICON_NEXT);
  heap_bitmap_init(&aw_menu_icons[AW_MENU_ICON_PLUS], RESOURCE_ID_MENU_ICON_PLUS);
  heap_bitmap_init(&aw_menu_icons[AW_MENU_ICON_MINUS], RESOURCE_ID_MENU_ICON_MINUS);
  heap_bitmap_init(&aw_menu_icons[AW_MENU_ICON_SAVE], RESOURCE_ID_MENU_ICON_SAVE);
  heap_bitmap_init(&aw_menu_icons[AW_MENU_ICON_TOGGLE], RESOURCE_ID_MENU_ICON_TOGGLE);

}

void aw_unload_bitmaps() {
  heap_bitmap_deinit(&aw_menu_icons[AW_MENU_ICON_NEXT]);
  heap_bitmap_deinit(&aw_menu_icons[AW_MENU_ICON_PLUS]);
  heap_bitmap_deinit(&aw_menu_icons[AW_MENU_ICON_MINUS]);
  heap_bitmap_deinit(&aw_menu_icons[AW_MENU_ICON_SAVE]);
  heap_bitmap_deinit(&aw_menu_icons[AW_MENU_ICON_TOGGLE]);
}

void aw_update_text() {
  static char aw_text_min[] = "00";
  snprintf(aw_text_min, sizeof(aw_text_min), "%02d", minutes);
  text_layer_set_text(&aw_tl_minutes, aw_text_min);

  static char aw_text_sec[] = "00";
  snprintf(aw_text_sec, sizeof(aw_text_sec), "%02d", seconds);
  text_layer_set_text(&aw_tl_seconds, aw_text_sec);

  static char aw_text_vibrate[50];
  switch (vibrate) {
    case TIMER_VIBE_OFF:
      strcpy(aw_text_vibrate, "No Vibe");
    break;
    case TIMER_VIBE_SHORT:
      strcpy(aw_text_vibrate, "Short Vibe");
    break;
    case TIMER_VIBE_LONG:
      strcpy(aw_text_vibrate, "Long Vibe");
    break;
    case TIMER_VIBE_DOUBLE:
      strcpy(aw_text_vibrate, "Double Vibe");
    break;
    case TIMER_VIBE_TRIPLE:
      strcpy(aw_text_vibrate, "Triple Vibe");
    break;
    case TIMER_VIBE_CONTINUOUS:
      strcpy(aw_text_vibrate, "Constant Vibe");
    break;
  }
  text_layer_set_text(&aw_tl_vibrate, aw_text_vibrate);

  switch (mode) {
    case AW_MODE_MINUTES:
    case AW_MODE_SECONDS:
    case AW_MODE_VIBE:
      text_layer_set_text(&aw_tl_hint, "");
    break;
    case AW_MODE_SAVE:
      text_layer_set_text(&aw_tl_hint, "Add timer ->");
    break;
  }
}

void aw_update_actionbar_icons() {
  switch (mode) {
    case AW_MODE_MINUTES:
    case AW_MODE_SECONDS:
    case AW_MODE_VIBE:
      action_bar_layer_set_icon(&aw_actionbar, BUTTON_ID_UP, &aw_menu_icons[AW_MENU_ICON_PLUS].bmp);
      action_bar_layer_set_icon(&aw_actionbar, BUTTON_ID_DOWN, &aw_menu_icons[AW_MENU_ICON_MINUS].bmp);
    break;
    case AW_MODE_SAVE:
      action_bar_layer_clear_icon(&aw_actionbar, BUTTON_ID_UP);
      action_bar_layer_set_icon(&aw_actionbar, BUTTON_ID_DOWN, &aw_menu_icons[AW_MENU_ICON_SAVE].bmp);
    break;
  }
  action_bar_layer_set_icon(&aw_actionbar, BUTTON_ID_SELECT, &aw_menu_icons[AW_MENU_ICON_NEXT].bmp);
  action_bar_layer_set_click_config_provider(&aw_actionbar, (ClickConfigProvider) aw_click_config_provider);
}

void aw_click_config_provider(ClickConfig **config, Window *window) {
  if (mode == AW_MODE_MINUTES || mode == AW_MODE_SECONDS || mode == AW_MODE_VIBE) {
    config[BUTTON_ID_UP]->click.handler = (ClickHandler) aw_up_clicked;
    config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
  }
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) aw_down_clicked;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) aw_select_clicked;
}


void aw_set_tick(AppContextRef ctx) {
  aw_timer_handle = app_timer_send_event(ctx, 500, COOKIE_AW_FLICKER);
}

void aw_cancel_tick(AppContextRef ctx) {
  app_timer_cancel_event(ctx, aw_timer_handle);
}

void add_window_timer() {
  switch (mode) {
    case AW_MODE_MINUTES:
      layer_set_hidden(&aw_tl_minutes.layer, ! layer_get_hidden(&aw_tl_minutes.layer));
    break;
    case AW_MODE_SECONDS:
      layer_set_hidden(&aw_tl_seconds.layer, ! layer_get_hidden(&aw_tl_seconds.layer));
    break;
    case AW_MODE_VIBE:
      layer_set_hidden(&aw_tl_vibrate.layer, ! layer_get_hidden(&aw_tl_vibrate.layer));
    break;
  }
  aw_set_tick(aw_app_ctx);
}

void aw_up_clicked(ClickRecognizerRef recognizer, Window* window) {
  switch (mode) {
    case AW_MODE_MINUTES:
      minutes += 1;
    break;
    case AW_MODE_SECONDS:
      seconds += 1;
    break;
    case AW_MODE_VIBE: {
      if (vibrate == TIMER_VIBE_CONTINUOUS) {
        vibrate = TIMER_VIBE_OFF;
      }
      else {
        vibrate += 1;
      }
    }
    break;
  }
  aw_fix_time();
  aw_update_text();
}


void aw_down_clicked(ClickRecognizerRef recognizer, Window* window) {
  switch (mode) {
    case AW_MODE_MINUTES:
      minutes -= 1;
    break;
    case AW_MODE_SECONDS:
      seconds -= 1;
    break;
    case AW_MODE_VIBE: {
      if (vibrate == TIMER_VIBE_OFF) {
        vibrate = TIMER_VIBE_CONTINUOUS;
      }
      else {
        vibrate -= 1;
      }
    }
    break;
    case AW_MODE_SAVE: {
      aw_create_timer();
      return;
    }
    break;
  }
  aw_fix_time();
  aw_update_text();
}

void aw_select_clicked(ClickRecognizerRef recognizer, Window* window) {
  switch (mode) {
    case AW_MODE_MINUTES: {
      mode = AW_MODE_SECONDS;
    }
    break;
    case AW_MODE_SECONDS: {
      mode = AW_MODE_VIBE;
    }
    break;
    case AW_MODE_VIBE:
      mode = AW_MODE_SAVE;
    break;
    case AW_MODE_SAVE:
      mode = AW_MODE_MINUTES;
    break;
  }
  aw_update_actionbar_icons();
  aw_update_text();
  aw_fix_hidden();
}

void aw_fix_time() {
  if (seconds < 0) {
    seconds += 60;
    minutes -= 1;
  }
  if (seconds >= 60) {
    seconds -= 60;
    minutes += 1;
  }
  if (minutes < 0) {
    minutes = 0;
  }
}

void aw_fix_hidden() {
  layer_set_hidden(&aw_tl_minutes.layer, false);
  layer_set_hidden(&aw_tl_seconds.layer, false);
  layer_set_hidden(&aw_tl_vibrate.layer, false);
}

void aw_reset() {
  mode = AW_MODE_MINUTES;
  minutes = 0;
  seconds = 0;
  vibrate = TIMER_VIBE_OFF;
  aw_update_actionbar_icons();
  aw_update_text();
}

void aw_create_timer() {
  int duration = minutes * 60 + seconds;
  if (duration == 0) {
    vibes_long_pulse();
    mode = AW_MODE_MINUTES;
    aw_update_actionbar_icons();
    aw_update_text();
  }
  else {
    add_timer(duration, vibrate);
    window_stack_pop(true);
  }
}