#include "../../src/settings.h"
#include "code.h"
#include "libs/linked-list/linked-list.h"

Settings _settings = {
  .save_timers_auto = true,
  .resume_timers = false,
  .timers_start_auto = false,
  .timers_vibration = TIMER_VIBRATION_SHORT,
  .timers_hours = false
};

Settings* settings(void) {
  return &_settings;
}

void win_vibrate_show(void) {
  }

bool win_vibrate_is_visible(void) {
  return false;
}

void win_main_update(void) {
}


typedef struct {
  char* group;
  char* operation;
  char* data;
} Message;

LinkedRoot* root = NULL;

void mqueue_add(char* group, char* operation, char* data) {
  if (NULL == root) {
    printf("mqueue not setup properly!\n");
    return;
  }
  Message* msg = malloc(sizeof(Message));
  msg->group = malloc(strlen(group) + 1);
  strcpy(msg->group, group);
  msg->operation = malloc(strlen(operation) + 1);
  strcpy(msg->operation, operation);
  msg->data = malloc(strlen(data) + 1);
  strcpy(msg->data, data);
  linked_list_append(root, msg);
}

void mqueue_reset(void) {
  if (NULL != root) {
    linked_list_clear(root);
    free(root);
  }
  root = linked_list_create_root();
}

char* mqueue_get_top_group(void) {
  if (NULL == root) {
    return NULL;
  }
  Message* msg = linked_list_get(root, 0);
  return msg->group;
}

char* mqueue_get_top_operation(void) {
  if (NULL == root) {
    return NULL;
  }
  Message* msg = linked_list_get(root, 0);
  return msg->operation;
}

char* mqueue_get_top_data(void) {
  if (NULL == root) {
    return NULL;
  }
  Message* msg = linked_list_get(root, 0);
  return msg->data;
}
