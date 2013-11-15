/***
 * Bitmap Loader
 * Copyright © 2013 Matthew Tole
 ***/

#include <pebble.h>
#include "bitmaps.h"

typedef struct {
  uint32_t res_id;
  GBitmap* bitmap;
} AppBitmap;

static AppBitmap* loaded_bitmaps;
static int bitmaps_loaded = 0;
static int bitmaps_count = 0;

void bitmaps_init(int count) {
  bitmaps_loaded = 0;
  bitmaps_count = count;
  loaded_bitmaps = malloc(sizeof(AppBitmap) * bitmaps_count);
}

GBitmap* bitmaps_get_bitmap(uint32_t res_id) {
  AppBitmap* app_bmp = NULL;
  for (int b = 0; b < bitmaps_loaded; b += 1) {
    if (loaded_bitmaps[b].res_id == res_id) {
      app_bmp = &loaded_bitmaps[b];
      break;
    }
  }
  if (app_bmp == NULL) {
    app_bmp = &loaded_bitmaps[bitmaps_loaded];
    bitmaps_loaded += 1;
    app_bmp->res_id = res_id;
    app_bmp->bitmap = gbitmap_create_with_resource(app_bmp->res_id);
  }
  return app_bmp->bitmap;
}

void bitmaps_cleanup(void) {
  for (int b = 0; b < bitmaps_loaded; b += 1) {
    gbitmap_destroy(loaded_bitmaps[b].bitmap);
  }
  free(loaded_bitmaps);
}
