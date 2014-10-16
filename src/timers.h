#include <pebble.h>
#include "timer.h"

#define TIMER_BLOCK_SIZE 4

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
Timer* timers_find_wakeup_collision(Timer* timer);

void timers_clear(void);

void timers_mark_updated(void);
void timers_highlight(Timer* timer);
void timers_register_update_handler(TimersUpdatedHandler handler);
void timers_register_highlight_handler(TimerHighlightHandler handler);

void timers_save(void);
void timers_restore(void);
void timers_migrate(void);