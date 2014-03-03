/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * common.h
 ***/

#pragma once

#include <pebble.h>

void uppercase(char* str);
void timer_duration_str(int duration, bool showHours, char* str, int str_len);