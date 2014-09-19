/*

Data Processor v1.1
A Pebble library for extracting elements from a delimited string.
http://smallstoneapps.github.io/data-processor/

----------------------

The MIT License (MIT)

Copyright © 2014 Matthew Tole

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

data-processor.h

*/

#pragma once

#include <pebble.h>

#define DATA_PROCESSOR_VERSION_MAJOR 1
#define DATA_PROCESSOR_VERSION_MINOR 1

typedef struct ProcessingState ProcessingState;

void data_processor_init(char* data, char delim);
ProcessingState* data_processor_create(char* data, char delim);

void data_processor_destroy(ProcessingState* state);
void data_processor_deinit();

ProcessingState* data_processor_get_global(void);

uint8_t data_processor_count(ProcessingState* state);
char* data_processor_get_string(ProcessingState* state);
bool data_processor_get_bool(ProcessingState* state);
int data_processor_get_int(ProcessingState* state);