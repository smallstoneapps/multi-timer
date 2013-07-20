#include "add-window.h"
#include "timers.h"

#define MENU_ROW_MINUTES 0
#define MENU_ROW_SECONDS 1
#define MENU_ROW_SAVE 2
#define MENU_ROW_RESET 3

void add_window_load(Window *me);
void add_window_unload(Window *me);
void add_window_appear(Window *me);

uint16_t addwin_menu_get_num_sections_callback(MenuLayer *me, void *data);
uint16_t addwin_menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
int16_t addwin_menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
void addwin_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void addwin_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void addwin_menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

MenuLayer addwin_layer_menu;
int minutes = 0;
int seconds = 0;
bool first_time = true;

void init_add_window(Window* wnd) {
  window_init(wnd, "Multi Timer Add Timer Window");
  window_set_window_handlers(wnd, (WindowHandlers){
    .load = add_window_load,
    .unload = add_window_unload
  });
}

void show_add_window(Window* wnd) {
  window_stack_push(wnd, true);
}

void add_window_load(Window *me) {
  if (! first_time) { return; }
  menu_layer_init(&addwin_layer_menu, me->layer.bounds);
  menu_layer_set_callbacks(&addwin_layer_menu, NULL, (MenuLayerCallbacks){
    .get_num_sections = addwin_menu_get_num_sections_callback,
    .get_num_rows = addwin_menu_get_num_rows_callback,
    .get_header_height = addwin_menu_get_header_height_callback,
    .draw_header = addwin_menu_draw_header_callback,
    .draw_row = addwin_menu_draw_row_callback,
    .select_click = addwin_menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(&addwin_layer_menu, me);
  layer_add_child(&me->layer, menu_layer_get_layer(&addwin_layer_menu));
  first_time = false;
}

void add_window_unload(Window *me) {

}

uint16_t addwin_menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 1;
}

uint16_t addwin_menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 4;
}

int16_t addwin_menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void addwin_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Add New Timer");
}

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
    case MENU_ROW_SAVE: {
      int length = (minutes * 60 + seconds);
      if (length <= 0) {
        return;
      }
      add_timer(length);
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
    case MENU_ROW_SAVE:
      menu_cell_basic_draw(ctx, cell_layer, "Save Timer", "Click to save the timer.", NULL);
    break;
    case MENU_ROW_RESET:
      menu_cell_basic_draw(ctx, cell_layer, "Reset", "Reset numbers to 0.", NULL);
    break;
  }
}
