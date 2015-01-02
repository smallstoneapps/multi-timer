#
# Pebble Tests v0.1.0
# A Pebble library for doing unit tests.
# http://smallstoneapps.github.io/pebble-tests/
#
# ----------------------
#
# The MIT License (MIT)
#
# Copyright © 2013 - 2015 Matthew Tole
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# --------------------
#
# Makefile
#

CC=gcc
ifeq ($(TRAVIS), true)
CFLAGS=-std=c99
else
CFLAGS=-std=c11
endif
CINCLUDES=-I tests/include/ -I tests/ -I src/ -I src/libs/linked-list/

TEST_FILES=tests/tests.c tests/timers.c
SRC_FILES=src/timers.c src/timer.c
LIB_FILES=src/libs/linked-list/linked-list.c
TEST_EXTRAS=tests/src/pebble.c tests/src/code.c

all: test

test:
	@printf "\n"
	@$(CC) $(CFLAGS) $(CINCLUDES) $(TEST_FILES) $(SRC_FILES) $(LIB_FILES) $(TEST_EXTRAS) -o tests/run
	@tests/run || (echo 'Multi Timer test suite failed.' | terminal-notifier; exit 1)
	@rm tests/run
	@printf "\x1B[0m"
	@printf "\n"

lint:
	oclint src/windows/win-main.c -- -c -std=c11 -I tests/include/ -I tests/ -I src/libs/pebble-assist/