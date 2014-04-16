/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * windows/win-error.h
 ***/

#pragma once

#include <pebble.h>

void win_error_init(void);
void win_error_set_text(char* text);
void win_error_show(void);
void win_error_destroy();