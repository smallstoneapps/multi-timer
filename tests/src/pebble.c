#include <pebble.h>

typedef struct Persist Persist;

typedef enum PersistType {
  DATA, NUMBER, STRING
} PersistType;

struct Persist {
  uint32_t key;
  PersistType type;
  void* data;
  uint32_t number;
  char* str;
  Persist* next;
};

static Persist* persist_read(uint32_t key);
static bool persist_write(Persist* persist);

Persist* persistence = NULL;

bool persist_exists(const uint32_t key) {
  return NULL != persist_read(key);
}

int persist_read_data(const uint32_t key, void *buffer, const size_t buffer_size) {
  Persist* persist = persist_read(key);
  if (! persist) {
    return -1;
  }
  if (DATA != persist->type) {
    return -1;
  }
  buffer = malloc(buffer_size);
  memcpy(buffer, persist->data, buffer_size);
  return buffer_size;
}

int32_t persist_read_int(const uint32_t key) {
  Persist* persist = persist_read(key);
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
  persist->next = NULL;
  persist_write(persist);
  return 0;
}

int persist_write_data(const uint32_t key, const void *data, const size_t size) {
  persist_delete(key);
  Persist* persist = malloc(sizeof(Persist));
  persist->key = key;
  persist->type = DATA;
  persist->data = malloc(size);
  memcpy(persist->data, data, size);
  persist->next = NULL;
  persist_write(persist);
  return size;
}

static bool persist_write(Persist* persist) {
  if (NULL == persistence) {
    persistence = persist;
  }
  else {
    Persist* ptr = persistence;
    while (NULL != ptr->next) {
      ptr = ptr->next;
    }
    ptr->next = persist;
  }
  return true;
}

static Persist* persist_read(uint32_t key) {
  Persist* ptr = persistence;
  while (NULL != ptr) {
    if (key == ptr->key) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}

status_t persist_delete(uint32_t key) {
  return S_SUCCESS;
}