/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)
Copyright © 2013 - 2015 Matthew Tole
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

src/icons.h

This file contains the GRect definitions for the icons contained in the image
resources/images/icons.png

*/

#pragma once

#define ICON_RECT_PLAY        (GRect) { {  0,  0 }, { 16, 16 } }
#define ICON_RECT_PAUSE       (GRect) { { 16,  0 }, { 16, 16 } }
#define ICON_RECT_STOP        (GRect) { { 32,  0 }, { 16, 16 } }
#define ICON_RECT_DONE        (GRect) { { 16, 16 }, { 16, 16 } }
#define ICON_RECT_TIMER       (GRect) { {  0, 16 }, {  8, 16 } }
#define ICON_RECT_STOPWATCH   (GRect) { {  8, 16 }, {  8, 16 } }
#define ICON_RECT_ADD         (GRect) { { 48, 16 }, { 16, 16 } }
#define ICON_RECT_RESET       (GRect) { { 48,  0 }, { 16, 16 } }
#define ICON_RECT_DELETE      (GRect) { {  0, 32 }, { 16, 16 } }
#define ICON_RECT_EDIT        (GRect) { { 16, 32 }, { 16, 16 } }
#define ICON_RECT_CONTROLS    (GRect) { {  0, 48 }, { 16, 16 } }
#define ICON_RECT_ABOUT       (GRect) { { 48, 32 }, { 16, 16 } }
#define ICON_RECT_SETTINGS    (GRect) { { 32, 32 }, { 16, 16 } }
#define ICON_RECT_ACTION_TICK (GRect) { { 16, 48 }, {  8,  8 } }
#define ICON_RECT_ACTION_INC  (GRect) { { 16, 56 }, {  8,  8 } }
#define ICON_RECT_ACTION_DEC  (GRect) { { 24, 48 }, {  8,  8 } }
#define ICON_RECT_CLOCK       (GRect) { { 32, 16 }, { 16, 16 } }
