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

src/timers.h

*/

#include <pebble.h>
#include "timer.h"

#define TIMER_BLOCK_SIZE 4

#define TIMERS_VERSION_CURRENT 3
#define TIMERS_VERSION_TINY 2

typedef void (*TimersUpdatedHandler)(void);
typedef void (*TimerHighlightHandler)(Timer* timer);

// Setup the timers module and its associated data objects.
// Must be called before all other operations.
void timers_init(void);

// Returns the number of timers.
uint8_t timers_count(void);

// Gets a single timer given its position.
// Returns NULL if there is no timer at that position.
Timer* timers_get(uint8_t index);

// Gets a single timer given its ID.
// Returns NULL if there is no timer with that ID.
Timer* timers_find(uint16_t id);

// Gets the index of a timer given its ID.
// Returns -1 if there is no timer with that ID.
int16_t timers_index_of(uint16_t id);

// Adds a new timer.
// Returns true if the operation was successful, false otherwise.
bool timers_add(Timer* timer);

// Removes a timer. This will destroy the timer object and all other
// associated memory.
// Returns true if successful, false otherwise (usually because there was
// no timer at that position).
bool timers_remove(uint8_t position);

// Return the timer that has the highest time_remaining value.
Timer* timers_find_last_wakeup(void);

// Return the timer that has a colliding timer with the specified one.
Timer* timers_find_wakeup_collision(Timer* timer);

// Empty list the timers.
void timers_clear(void);

void timers_mark_updated(void);
void timers_highlight(Timer* timer);
void timers_register_update_handler(TimersUpdatedHandler handler);
void timers_register_highlight_handler(TimerHighlightHandler handler);

void timers_save(void);
void timers_restore(void);
