/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 ***/

#include <pebble_os.h>
#include "bitmaps.h"

#define MAX_BITMAPS 10

typedef struct {
  uint32_t res_id;
  GBitmap* bitmap;
} AppBitmap;

static AppBitmap loaded_bitmaps[MAX_BITMAPS];
static int bitmaps_loaded = 0;

void bitmaps_init(void) {
  bitmaps_loaded = 0;
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
}
