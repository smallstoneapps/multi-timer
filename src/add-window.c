#include "add-window.h"
#include "timers.h"

<<<<<<< Updated upstream
#define MENU_ROW_MINUTES 0
#define MENU_ROW_SECONDS 1
#define MENU_ROW_VIBRATE 2
#define MENU_ROW_SAVE 3
#define MENU_ROW_RESET 4
=======
#define TL_MINUTE_NUMBER 0 
#define TL_MINUTE_LABEL 1
#define TL_SECOND_NUMBER 2
#define TL_SECOND_LABEL 3
#define TL_VIBRATION 4
>>>>>>> Stashed changes

void add_window_load(Window *me);
void add_window_unload(Window *me);
void add_window_appear(Window *me);

TextLayer addwin_text_layers[5];
TextLayer addwin_layer_text_seconds;
ActionBarLayer addwin_action_bar;
int minutes = 0;
int seconds = 0;
<<<<<<< Updated upstream
bool first_time = true;
bool vibrate = true;
=======
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
uint16_t addwin_menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 5;
=======
void reset_add_window(Window* me) {
  minutes = 0;
  seconds = 0;
>>>>>>> Stashed changes
}

// PRIVATE FUNCTIONS

void add_window_load(Window *me) {
  action_bar_layer_add_to_window(&addwin_action_bar, me);
}

<<<<<<< Updated upstream
void addwin_menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->row) {
    case MENU_ROW_MINUTES: {
      minutes += 1;
      if (minutes > 60) {
        minutes = 0;
      }
      menu_layer_reload_data(&addwin_layer_menu);
    }
    break;
    case MENU_ROW_SECONDS: {
      seconds += 1;
      if (seconds >= 60) {
        seconds = 0;
      }
      menu_layer_reload_data(&addwin_layer_menu);
    }
    break;
    case MENU_ROW_VIBRATE: {
      vibrate = ! vibrate;
      menu_layer_reload_data(&addwin_layer_menu);
    }
    break;
    case MENU_ROW_SAVE: {
      int length = (minutes * 60 + seconds);
      if (length <= 0) {
        return;
      }
      add_timer(length, vibrate);
      minutes = 0;
      seconds = 0;
      window_stack_pop(true);
    }
    break;
    case MENU_ROW_RESET: {
      minutes = 0;
      seconds = 0;
      menu_layer_reload_data(&addwin_layer_menu);
    }
    break;
  }
}

void addwin_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->row) {
    case MENU_ROW_MINUTES: {
      char minutes_str[20];
      snprintf(minutes_str, sizeof(minutes_str), "%d minutes", minutes);
      menu_cell_basic_draw(ctx, cell_layer, minutes_str, "Click to increment.", NULL);
    }
    break;
    case MENU_ROW_SECONDS: {
      char seconds_str[20];
      snprintf(seconds_str, sizeof(seconds_str), "%d seconds", seconds);
      menu_cell_basic_draw(ctx, cell_layer, seconds_str, "Click to increment.", NULL);
    }
    break;
    case MENU_ROW_VIBRATE: {
      if (vibrate) {
        menu_cell_basic_draw(ctx, cell_layer, "Vibrate Enabled", "Click to disable vibration.", NULL);
      }
      else {
        menu_cell_basic_draw(ctx, cell_layer, "Vibrate Disabled", "Click to enable vibration.", NULL);
      }
    }
    break;
    case MENU_ROW_SAVE:
      menu_cell_basic_draw(ctx, cell_layer, "Save Timer", "Click to save the timer.", NULL);
    break;
    case MENU_ROW_RESET:
      menu_cell_basic_draw(ctx, cell_layer, "Reset", "Reset numbers to 0.", NULL);
    break;
  }
}
=======
void add_window_unload(Window *me) {
}

void add_window_appear(Window *me) {

}
>>>>>>> Stashed changes
