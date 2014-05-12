#include <pebble.h>
#include "../src/timer.h"
#include "../src/settings.h"

void timer_resume(Timer* timer) {

}

void timer_reset(Timer* timer) {

}

void timer_destroy(Timer* timer) {
  free(timer);
}

Settings* settings(void) {
  return NULL;
}
