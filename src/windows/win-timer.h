#pragma once

#include <pebble.h>
#include "../timer.h"

void win_timer_init(void);
void win_timer_set_timer(Timer* timer);
void win_timer_show(void);