/***
 * Data Processor
 * Copyright © 2014 Matthew Tole
 ***/

#pragma once

#include <pebble.h>

void data_processor_init(char* data, char delim);
uint8_t data_processor_count(void);
char* data_processor_get_string(void);
bool data_processor_get_bool(void);
int data_processor_get_int(void);