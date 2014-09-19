/*

Multi Timer v3.0

http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)

Copyright © 2013 - 2014 Matthew Tole

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

src/timers.h

*/

#pragma once

#include "timer.h"

#define MAX_TIMERS 16

void timers_init(void);
Timer* timers_get(uint8_t pos);
int timers_get_count();
void timers_clear();
void timers_add(Timer* timer);
void timers_remove(uint8_t pos);
Timer* timers_find(const uint16_t id);

status_t timers_restore(void);
status_t timers_save(void);

void timers_send_list(void);
void timers_load_list(char* data);
void timers_get_list(void);
bool timers_are_blank(void);
