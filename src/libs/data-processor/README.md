# Data Processor

Pebble library to extract strings, ints and booleans from a long string.

## Usage

````c
// This is not a complete example,
// but should demonstrate the basic usage of Bitmap Loader.

static void parse_data(char* data) {
  data_processor_init(data, '|');
  uint8_t num_strings = data_processor_count();
  char** strings = malloc(sizeof(char*) * num_strings);
  for (uint8_t n = 0; n < num_strings; n += 1) {
    data_processor_get_string(&strings[n]);
  }
}
````

## Function Documentation

Initialise the library. Takes the long string and the delimiter character as arguments.

    bool data_processor_init(char* data, char delim);

Returns the number of elements in the long string.

    uint8_t data_processor_count(void);

Load the next substring from the long string. Takes a pointer to a string as an argument.

    bool data_processor_get_string(char** str);

Load the next item as a boolean from the long string. Takes a pointer to a boolean as an argument.

    bool data_processor_get_bool(bool* boolean);

Load the next item as an int from the long string. Takes a pointer to a uint8_t as an argument.

    bool data_processor_get_uint8(uint8_t* num);