/***
 * Data Processor
 * Copyright © 2014 Matthew Tole
 ***/

#include <pebble.h>
#include "data-processor.h"

static char* data_start = NULL;
static char* data_pos = NULL;
static char data_delim;

void data_processor_init(char* data, char delim) {
  data_start = data;
  data_pos = data_start;
  data_delim = delim;
}

uint8_t data_processor_count(void) {
  char* pos = data_start;
  uint8_t count = 0;
  while (*pos != '\0') {
    if (*pos == data_delim) {
      count += 1;
    }
    pos++;
  }
  return ++count;
}

char* data_processor_get_string(void) {
  char* data_block_start = data_pos;
  int str_len = 0;
  while (*data_pos != data_delim && *data_pos != '\0') {
    data_pos++;
    str_len += 1;
  }
  *data_pos = '\0';
  char* tmp = malloc(str_len + 1);
  strncpy(tmp, data_block_start, str_len + 1);
  data_pos++;
  return tmp;
}

bool data_processor_get_bool(void) {
  char bool_char =  *data_pos;
  data_pos++;
  data_pos++;
  return (bool_char == '1');
}

int data_processor_get_int(void) {
  char* tmp_str = data_processor_get_string();
  int num = atoi(tmp_str);
  free(tmp_str);
  return num;
}

