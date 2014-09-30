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
#include <linked-list.h>
#include "bitmap-loader.h"

typedef struct {
  uint32_t res_id;
  GBitmap* bitmap;
  uint8_t group;
  GRect* rect;
  bool is_sub;
} AppBitmap;

static AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id);
static AppBitmap* get_app_bitmap_by_group(uint8_t group);
static AppBitmap* get_app_bitmap_tail(void);

static LinkedRoot* bitmaps = NULL;

void bitmaps_init() {
  bitmaps_cleanup();
  bitmaps = linked_list_create_root();
}

GBitmap* bitmaps_get_bitmap(uint32_t res_id) {
  if (! bitmaps) {
    return NULL;
  }
  AppBitmap* app_bmp = get_app_bitmap_by_res_id(res_id);
  if (app_bmp == NULL) {
    app_bmp = malloc(sizeof(AppBitmap));
    app_bmp->res_id = res_id;
    app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
    app_bmp->group = 0;
    app_bmp->is_sub = false;
    linked_list_append(bitmaps, app_bmp);
  }
  return app_bmp->bitmap;
}

GBitmap* bitmaps_get_bitmap_in_group(uint32_t res_id, uint8_t group) {
  if (! bitmaps) {
    return NULL;
  }
  if (group <= 0) {
    return NULL;
  }
  AppBitmap* app_bmp = get_app_bitmap_by_group(group);
  if (NULL == app_bmp) {
    app_bmp = malloc(sizeof(AppBitmap));
    app_bmp->res_id = res_id;
    app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
    app_bmp->group = group;
    app_bmp->is_sub = false;
    linked_list_append(bitmaps, app_bmp);
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

GBitmap* bitmaps_get_sub_bitmap(uint32_t res_id, GRect rect) {
  if (! bitmaps) {
    return NULL;
  }
  GBitmap* parent = NULL;
  uint8_t count = linked_list_count(bitmaps);
  for (uint8_t b = 0; b < count; b += 1) {
    AppBitmap* bmp = (AppBitmap*)linked_list_get(bitmaps, b);
    if (bmp->res_id == res_id) {
      if (bmp->is_sub) {
        if (grect_equal(bmp->rect, &rect)) {
          return bmp->bitmap;
        }
      }
      else {
        parent = bmp->bitmap;
      }
    }
  }
  if (! parent) {
    parent = bitmaps_get_bitmap(res_id);
    if (! parent) {
      return NULL;
    }
  }
  AppBitmap* app_bmp = malloc(sizeof(AppBitmap));
  if (app_bmp == NULL) {
    return NULL;
  }
  app_bmp->res_id = res_id;
  app_bmp->bitmap = gbitmap_create_as_sub_bitmap(parent, rect);
  if (app_bmp->bitmap == NULL) {
    return NULL;
  }
  app_bmp->rect = malloc(sizeof(GRect));
  app_bmp->rect->origin = rect.origin;
  app_bmp->rect->size = rect.size;
  app_bmp->is_sub = true;
  linked_list_append(bitmaps, app_bmp);
  return app_bmp->bitmap;
}

void bitmaps_cleanup(void) {
  while (linked_list_count(bitmaps) > 0) {
    AppBitmap* bmp = linked_list_get(bitmaps, 0);
    gbitmap_destroy(bmp->bitmap);
    free(bmp);
    linked_list_remove(bitmaps, 0);
  }
}

// - - -

static AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id) {
  uint8_t count = linked_list_count(bitmaps);
  for (uint8_t b = 0; b < count; b += 1) {
    AppBitmap* bmp = (AppBitmap*)linked_list_get(bitmaps, b);
    if (bmp->res_id == res_id) {
      return bmp;
    }
  }
  return NULL;
}

static AppBitmap* get_app_bitmap_by_group(uint8_t group) {
  uint8_t count = linked_list_count(bitmaps);
  for (uint8_t b = 0; b < count; b += 1) {
    AppBitmap* bmp = (AppBitmap*)linked_list_get(bitmaps, b);
    if (bmp->group == group) {
      return bmp;
    }
  }
  return NULL;
}
