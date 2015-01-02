/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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

--------------------

src/main.c

*/

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
