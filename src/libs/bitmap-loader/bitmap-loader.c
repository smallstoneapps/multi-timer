/***
 * Bitmap Loader
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * Version 2.0.2
 ***/

#include <pebble.h>
#include "bitmap-loader.h"

typedef struct AppBitmap AppBitmap;

struct AppBitmap {
  uint32_t res_id;
  GBitmap* bitmap;
  AppBitmap* next;
};

static AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id);
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

void bitmaps_cleanup(void) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    gbitmap_destroy(current->bitmap);
    AppBitmap* tmp = current;
    current = tmp->next;
    free(current);
  }
  bitmaps = NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

AppBitmap* get_app_bitmap_by_res_id(uint32_t res_id) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    if (current->res_id == res_id) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

AppBitmap* get_app_bitmap_tail(void) {
  AppBitmap* current = bitmaps;
  while (current != NULL) {
    if (current->next == NULL) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}