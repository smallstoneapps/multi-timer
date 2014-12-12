#include <pebble.h>
#include <bitmap-loader.h>
#include "windows/win-main.h"
#include "timers.h"
#include "settings.h"
#include "persist.h"

static void init(void);
static void deinit(void);

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void init(void) {
  srand(time(NULL));
  timers_init();
  bitmaps_init();
  settings_load();
  timers_restore();
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  win_main_init();
  win_main_show();
}

static void deinit(void) {
  timers_save();
  settings_save();
}