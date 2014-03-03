# Bitmap Loader

Pebble library to do lazy loading of bitmaps from resources

## What does it do?

Bitmap Loader will automatically load bitmaps when they are needed, rather than having to handle the loading of them yourself. 

The function `bitmaps_get_bitmap` takes a *RESOURCE_ID* as an argument, and returns a pointer to a *GBitmap*. If this is the first time that the bitmap has been requested, it will load the image from resources. 

## Usage

````c
// This is not a complete example, but should demonstrate the basic usage of Bitmap Loader.

static void init(void) {
  bitmaps_init();
}

static void deiinit(void) {
  bitmaps_cleanup();
}

static void window_load(Window* window) {
  bitmap_layer_set_bitmap(bitmaps_get_bitmap(RESOURCE_ID_BITMAP_1));
  GBitmap* bitmap = (bitmaps_get_bitmap(RESOURCE_ID_BITMAP_2));
}
````

## Function Documentation

Initialise the Bitmap Loader

**Note:** Call this once, preferably in your *init* function.

    void bitmaps_init(void);

Get the bitmap using its resource ID.

    GBitmap* bitmaps_get_bitmap(uint32_t res_id);

Cleanup the Bitmap Loader.

**Note** Call this in your *deinit* function.

    void bitmaps_cleanup(void);
