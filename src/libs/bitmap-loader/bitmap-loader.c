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

----------------------

src/bitmap-loader.c

*/

#include <pebble.h>
#include "bitmap-loader.h"

typedef struct AppBitmap AppBitmap;

struct AppBitmap {
  uint32_t res_id;
  GBitmap* bitmap;
  uint8_t group;
  AppBitmap* next;
};

static AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id);
static AppBitmap* get_app_bitmap_by_group(uint8_t group);
static AppBitmap* get_app_bitmap_tail(void);

static AppBitmap* bitmaps = NULL;

void bitmaps_init() {
  bitmaps_cleanup();
  bitmaps = NULL;
}

GBitmap* bitmaps_get_bitmap(uint32_t res_id) {
  AppBitmap* app_bmp = get_app_bitmap_by_res_id(res_id);
  if (app_bmp == NULL) {
    app_bmp = malloc(sizeof(AppBitmap));
    app_bmp->res_id = res_id;
    app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
    app_bmp->group = 0;
    app_bmp->next = NULL;
    AppBitmap* last = get_app_bitmap_tail();
    if (last == NULL) {
      bitmaps = app_bmp;
    }
    else {
      last->next = app_bmp;
    }
  }
  return app_bmp->bitmap;
}

GBitmap* bitmaps_get_bitmap_in_group(uint32_t res_id, uint8_t group) {
  if (group <= 0) {
    return NULL;
  }
  AppBitmap* app_bmp = get_app_bitmap_by_group(group);
  if (NULL == app_bmp) {
    app_bmp = malloc(sizeof(AppBitmap));
    app_bmp->res_id = res_id;
    app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
    app_bmp->group = group;
    app_bmp->next = NULL;
    AppBitmap* last = get_app_bitmap_tail();
    if (last == NULL) {
      bitmaps = app_bmp;
    }
    else {
      last->next = app_bmp;
    }
  }
  else {
    if (res_id != app_bmp->res_id) {
      gbitmap_destroy(app_bmp->bitmap);
      app_bmp->res_id = res_id;
      app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
    }
  }
  return app_bmp->bitmap;
}

void bitmaps_cleanup(void) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    gbitmap_destroy(current->bitmap);
    AppBitmap* tmp = current;
    current = current->next;
    free(tmp);
  }
  bitmaps = NULL;
}

// - - -

static AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    if (current->res_id == res_id) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

static AppBitmap* get_app_bitmap_by_group(uint8_t group) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    if (current->group == group) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

static AppBitmap* get_app_bitmap_tail(void) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    if (current->next == NULL) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}