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

src/common.c

*/

#include <pebble.h>
#include "globals.h"

void uppercase(char* str) {
  char* point = str;
  while (*point != '\0') {
    if (*point >= 97 && *point <= 122) {
      *point -= 32;
    }
    point += 1;
  }
}

//! Test whether or not the persistent storage is working, by writing
//! and reading a value.
//! @return status code of write, negative values are bad.
StatusCode test_persist(void) {
  StatusCode write_status = persist_write_bool(STORAGE_TEST, true);
  bool test_bool = persist_read_bool(STORAGE_TEST);
  if (true != test_bool) {
    return -100;
  }
  return write_status;
}
