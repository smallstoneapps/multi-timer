# ScrollTextLayer

Pebble library for easily adding a scrollable text layer to your app.

## Usage

````c
// This is not a complete example, but should demonstrate the basic usage of a ScrollTextLayer.

static ScrollTextLayer* scroll_text;
static char* long_text = "Bacon ipsum dolor sit amet ribeye brisket leberkas, frankfurter pork loin salami biltong. \nPig spare ribs drumstick filet mignon tongue, rump pork belly andouille ball tip shoulder chuck sausage. Beef rump pig hamburger ham hock. Hamburger landjaeger cow, frankfurter drumstick ball tip pork swine tongue shoulder prosciutto t-bone flank chicken. \nTri-tip shoulder tongue pork belly, beef ribs pancetta flank tenderloin. Capicola hamburger t-bone ground round spare ribs rump, brisket tongue flank ball tip short ribs. Beef pork chop flank pork belly pig chuck.";

static void window_load(Window* window) {
  scroll_text = scroll_text_layer_create_fullscreen(window);
  scroll_text_layer_set_system_font(scroll_text, FONT_KEY_GOTHIC_18);
  scroll_text_layer_set_text(scroll_text, long_text);
  scroll_text_layer_add_to_window(scroll_text, window);
}
````

## Function Documentation

### Basic Functions

Create a new ScrollTextLayer.

    ScrollTextLayer* scroll_text_layer_create(GRect rect);

Destroy a ScrollTextLayer.

    void scroll_text_layer_destroy(ScrollTextLayer* layer);

Get the pointer to the internal TextLayer for a ScrollTextLayer.

    TextLayer* scroll_text_layer_get_text_layer(ScrollTextLayer* layer);

Get the pointer to the internal ScrollLayer for a ScrollTextLayer.

    ScrollLayer* scroll_text_layer_get_scroll_layer(ScrollTextLayer* layer);

Add the ScrollTextLayer to the window.

    void scroll_text_layer_add_to_window(ScrollTextLayer* layer, Window* window);

Set the text for the ScrollTextLayer.
**Note:** This behaves in the same was as a normal TextLayer, so you must responsible for keeping the text in memory (e.g. making it a static variable)

    void scroll_text_layer_set_text(ScrollTextLayer* layer, char* text);

### Utility Functions

Create a new ScrollTextLayer that is the size of a window.

    ScrollTextLayer* scroll_text_layer_create_fullscreen(window) scroll_text_layer_create(layer_get_bounds(window_get_root_layer(window)));

Set the text colour of the ScrollTextLayer.

    void scroll_text_layer_set_text_color(layer, color) text_layer_set_text_color(scroll_text_layer_get_text_layer(layer), color)

Set the background colour of the ScrollTextLayer.

    void scroll_text_layer_set_background_color(layer, color) text_layer_set_background_color(scroll_text_layer_get_text_layer(layer), color)

Set the font of the ScrollTextLayer.

    void scroll_text_layer_set_font(layer, font) text_layer_set_font(scroll_text_layer_get_text_layer(layer), font)

Set the text alignment of a ScrollTextLayer.

    void scroll_text_layer_set_text_alignment(layer, alignment) text_layer_set_text_alignment(scroll_text_layer_get_text_layer(layer), alignment)

Set the font of the ScrollTextLayer to a system font.

    void scroll_text_layer_set_system_font(layer, font) scroll_text_layer_get_text_layer(layer, fonts_get_system_font(font))
