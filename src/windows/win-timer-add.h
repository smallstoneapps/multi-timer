#pragma once

#include <pebble.h>

#include "timer.h"

void win_timer_add_init(void);
void win_timer_add_show_new(void);
void win_timer_add_show_edit(Timer* timer);