/***
 * Data Processor
 * Copyright © 2014 Matthew Tole
 ***/

#pragma once

#include <pebble.h>

bool data_processor_init(char* data, char delim);
uint8_t data_processor_count(void);
bool data_processor_get_string(char** str);
bool data_processor_get_bool(bool* boolean);
bool data_processor_get_uint8(uint8_t* num);