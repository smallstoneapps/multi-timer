#include "add-window.h"
#include "timers.h"

#define TL_MINUTE_NUMBER 0
#define TL_MINUTE_LABEL 1
#define TL_SECOND_NUMBER 2
#define TL_SECOND_LABEL 3
#define TL_VIBRATION 4

void add_window_load(Window *me);
void add_window_unload(Window *me);
void add_window_appear(Window *me);

TextLayer addwin_text_layers[5];
TextLayer addwin_layer_text_seconds;
ActionBarLayer addwin_action_bar;
int minutes = 0;
int seconds = 0;
bool vibrate = true;

// PUBLIC FUNCTIONS

void init_add_window(Window* me) {
  window_init(me, "Multi Timer Add Timer Window");
  window_set_window_handlers(me, (WindowHandlers){
    .load = add_window_load,
    .unload = add_window_unload,
    .appear = add_window_appear,
  });

  text_layer_init(&addwin_text_layers[TL_MINUTE_NUMBER], GRect(8, 8, 144 - ACTION_BAR_WIDTH - 8, 42));
  text_layer_set_text_color(&addwin_text_layers[TL_MINUTE_NUMBER], GColorBlack);
  text_layer_set_background_color(&addwin_text_layers[TL_MINUTE_NUMBER], GColorClear);
  text_layer_set_font(&addwin_text_layers[TL_MINUTE_NUMBER], fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(&addwin_text_layers[TL_MINUTE_NUMBER], GTextAlignmentLeft);
  text_layer_set_text(&addwin_text_layers[TL_MINUTE_NUMBER], "00");
  layer_add_child(&me->layer, &addwin_text_layers[TL_MINUTE_NUMBER].layer);

  text_layer_init(&addwin_text_layers[TL_MINUTE_LABEL], GRect(8, 46, 144 - ACTION_BAR_WIDTH - 8, 18));
  text_layer_set_text_color(&addwin_text_layers[TL_MINUTE_LABEL], GColorBlack);
  text_layer_set_background_color(&addwin_text_layers[TL_MINUTE_LABEL], GColorClear);
  text_layer_set_font(&addwin_text_layers[TL_MINUTE_LABEL], fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&addwin_text_layers[TL_MINUTE_LABEL], GTextAlignmentLeft);
  text_layer_set_text(&addwin_text_layers[TL_MINUTE_LABEL], "minutes");
  layer_add_child(&me->layer, &addwin_text_layers[TL_MINUTE_LABEL].layer);

  action_bar_layer_init(&addwin_action_bar);
}

void show_add_window(Window* me) {
  window_stack_push(me, true);
}

void reset_add_window(Window* me) {
  minutes = 0;
  seconds = 0;
}

// PRIVATE FUNCTIONS

void add_window_load(Window *me) {
  action_bar_layer_add_to_window(&addwin_action_bar, me);
}

void add_window_unload(Window *me) {
}

void add_window_appear(Window *me) {

}