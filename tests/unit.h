/*

Pebble Tests
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

tests/unit.h

This file is based on minunit.h, which was found here on
http://www.jera.com/techinfo/jtns/jtn002.html as has been modified for my
purposes.

*/

#define mu_assert(test, message) do { \
  if (!(test)) { \
    return message; \
  } \
} while (0)

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