/*

Pebble Tests v0.1.0
A Pebble library for doing unit tests.
http://smallstoneapps.github.io/pebble-tests/

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

unit.h

This file is based on minunit.h, which was found here
http://www.jera.com/techinfo/jtns/jtn002.html
and has been modified for my purposes.

*/

// Colour code definitions to make the output all pretty.
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define mu_assert(test, message) do { \
  if (!(test)) { \
    return message; \
  } \
} while (0)

#define mu_run_test_group(group) do { \
  char* message = group(); \
  if (message) { \
    return message; \
  } \
} while(0)

#define mu_run_test(test) do { \
  before_each(); \
  char *message = test(); \
  after_each(); \
  tests_run++; \
  if (message) { \
    return message; \
  } \
  tests_passed++; \
} while (0)

extern int tests_run;
extern int tests_passed;

void none(void);
void before_each(void);
void after_each(void);
