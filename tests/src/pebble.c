/*

Pebble Tests v0.1.0
A Pebble library for doing unit tests.
http://smallstoneapps.github.io/pebble-tests/

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

pebble.c

*/

#include <pebble.h>
#include "pebble_extra.h"
#include "libs/linked-list/linked-list.h"

typedef enum PersistType {
  DATA, NUMBER, STRING
} PersistType;

typedef struct Persist {
  uint32_t key;
  PersistType type;
  void* data;
  uint32_t number;
  char* str;
} Persist;

static bool persist_compare(void* persist1, void* persist2);

LinkedRoot* persistence = NULL;

void persist_reset(void) {
  linked_list_clear(persistence);
}

void persist_clear(void) {
  linked_list_clear(persistence);
}

void persist_init(void) {
  persistence = linked_list_create_root();
}

Persist* fake_persist(const uint32_t key) {
  Persist* p = malloc(sizeof(Persist));
  p->key = key;
  return p;
}

static bool persist_compare(void* persist1, void* persist2) {
  return ((Persist*)persist1)->key == ((Persist*)persist2)->key;
}

bool persist_exists(const uint32_t key) {
  return linked_list_contains_compare(persistence, fake_persist(key), persist_compare);
}

int persist_read_data(const uint32_t key, void *buffer, const size_t buffer_size) {
  Persist* persist = (Persist*) linked_list_get(persistence, linked_list_find_compare(persistence, fake_persist(key), persist_compare));
  if (! persist) {
    return -1;
  }
  if (DATA != persist->type) {
    return -1;
  }
  memcpy(buffer, persist->data, buffer_size);
  return buffer_size;
}

int32_t persist_read_int(const uint32_t key) {
  Persist* persist = (Persist*) linked_list_get(persistence, linked_list_find_compare(persistence, fake_persist(key), persist_compare));
  if (! persist) {
    return -1;
  }
  if (NUMBER != persist->type) {
    return -1;
  }
  return persist->number;
}

status_t persist_write_int(const uint32_t key, const int32_t value) {
  persist_delete(key);
  Persist* persist = malloc(sizeof(Persist));
  persist->key = key;
  persist->type = NUMBER;
  persist->number = value;
  linked_list_append(persistence, persist);
  return 0;
}

int persist_write_data(const uint32_t key, const void *data, const size_t size) {
  persist_delete(key);
  Persist* persist = malloc(sizeof(Persist));
  persist->key = key;
  persist->type = DATA;
  persist->data = malloc(size);
  memcpy(persist->data, data, size);
  linked_list_append(persistence, persist);
  return size;
}

int persist_read_string(const uint32_t key, char *buffer, const size_t buffer_size) {
  Persist* persist = (Persist*) linked_list_get(persistence, linked_list_find_compare(persistence, fake_persist(key), persist_compare));
  if (! persist) {
    return -1;
  }
  if (STRING != persist->type) {
    return -1;
  }
  strncpy(buffer, persist->str, buffer_size);
  return buffer_size;
}

int persist_write_string(const uint32_t key, const char *cstring) {
  persist_delete(key);
  Persist* persist = malloc(sizeof(Persist));
  persist->key = key;
  persist->type = STRING;
  persist->str = malloc(strlen(cstring) + 1);
  strcpy(persist->str, cstring);
  linked_list_append(persistence, persist);
  return strlen(cstring);
}

status_t persist_delete(uint32_t key) {
  linked_list_remove(persistence, linked_list_find_compare(persistence, fake_persist(key), persist_compare));
  return S_SUCCESS;
}

void app_log(uint8_t log_level, const char *src_filename, int src_line_number, const char *fmt, ...) {

}

uint32_t app_message_inbox_size_maximum(void) {
  return APP_MESSAGE_INBOX_SIZE_MINIMUM;
}

uint32_t app_message_outbox_size_maximum(void) {
  return APP_MESSAGE_OUTBOX_SIZE_MINIMUM;
}

AppMessageResult app_message_open(const uint32_t size_inbound, const uint32_t size_outbound) {
  return APP_MSG_OK;
}

AppMessageInboxReceived app_message_register_inbox_received(AppMessageInboxReceived received_callback) {
  return APP_MSG_OK;
}

AppMessageInboxDropped app_message_register_inbox_dropped(AppMessageInboxDropped dropped_callback) {
  return APP_MSG_OK;
}

AppMessageOutboxSent app_message_register_outbox_sent(AppMessageOutboxSent sent_callback) {
  return APP_MSG_OK;
}

AppMessageOutboxFailed app_message_register_outbox_failed(AppMessageOutboxFailed failed_callback) {
  return APP_MSG_OK;
}

AppMessageResult app_message_outbox_begin(DictionaryIterator **iterator) {
  return APP_MSG_OK;
}

AppMessageResult app_message_outbox_send(void) {
  return APP_MSG_OK;
}

Tuple *dict_find(const DictionaryIterator *iter, const uint32_t key) {
  return NULL;
}

DictionaryResult dict_write_cstring(DictionaryIterator *iter, const uint32_t key, const char * const cstring) {
  return DICT_OK;
}

void app_timer_cancel(AppTimer *timer_handle) {

}

AppTimer* app_timer_register(uint32_t timeout_ms, AppTimerCallback callback, void* callback_data) {
  return NULL;
}

GFont fonts_get_system_font(const char *font_key) {
  return NULL;
}

GBitmap* gbitmap_create_with_resource(uint32_t resource_id) {
  return NULL;
}

GBitmap* gbitmap_create_as_sub_bitmap(const GBitmap *base_bitmap, GRect sub_rect) {
  return NULL;
}

void gbitmap_destroy(GBitmap* bitmap) {

}

void graphics_context_set_text_color(GContext* ctx, GColor color) {

}

void graphics_draw_bitmap_in_rect(GContext* ctx, const GBitmap *bitmap, GRect rect) {

}

void graphics_draw_text(GContext* ctx, const char* text, GFont const font, const GRect box, const GTextOverflowMode overflow_mode, const GTextAlignment alignment, const GTextLayoutCacheRef layout) {

}

void vibes_enqueue_custom_pattern(VibePattern pattern) {

}

void vibes_long_pulse(void) {

}

void vibes_short_pulse(void) {
  
}