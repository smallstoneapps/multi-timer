/*

Bitmap Loader v2.1
On-demand loading of bitmaps from resources.
http://smallstoneapps.github.io/bitmap-loader/

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

src/bitmap-loader.h

*/

#pragma once

#include <pebble.h>

// Initialise the bitmap loading library.
void bitmaps_init(void);

// Returns a pointer to the bitmap as specified by a resource ID.
// If the bitmap has not been requested before, it will be loaded.
GBitmap* bitmaps_get_bitmap(uint32_t res_id);

// Returns a pointer to the bitmap as specified by a resource ID.
// If the bitmap has not been requested before, it will be loaded.
// If another bitmap has been loaded with the same group number, it will be
// unloaded.
GBitmap* bitmaps_get_bitmap_in_group(uint32_t res_id, uint8_t group);

GBitmap* bitmaps_get_sub_bitmap(uint32_t res_id, GRect rect);

// Unload all the bitmaps from memory.
void bitmaps_cleanup(void);
