/*

Linked List v0.3
A Pebble library for working with linked lists.
http://smallstoneapps.github.io/linked-list/

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

src/linked-list.h

*/

#include <pebble.h>
#include "linked-list.h"

typedef struct LinkedList LinkedList;

struct LinkedList {
  LinkedList* next;
  LinkedList* prev;
  void* object;
};

struct LinkedRoot {
  LinkedList* head;
};

static LinkedList* create_list_item(void* object);
static LinkedList* list_get(LinkedRoot* root, uint16_t index);
static bool pointer_compare(void* p1, void* p2);

LinkedRoot* linked_list_create_root(void) {
  LinkedRoot* root = malloc(sizeof(LinkedRoot));
  root->head = NULL;
  return root;
}

uint16_t linked_list_count(LinkedRoot* root) {
  if (NULL == root) {
    return 0;
  }
  LinkedList* list = root->head;
  uint16_t size = 0;
  while (NULL != list) {
    list = list->next;
    size += 1;
  }
  return size;
}

void linked_list_append(LinkedRoot* root, void* object) {
  if (NULL == root) {
    return;
  }
  LinkedList* child = create_list_item(object);
  if (NULL == root->head) {
    root->head = child;
  }
  else {
    LinkedList* tail = root->head;
    while (NULL != tail->next) {
      tail = tail->next;
    }
    tail->next = child;
    child->prev = tail;
  }
}

void linked_list_prepend(LinkedRoot* root, void* object) {
  if (NULL == root) {
    return;
  }
  LinkedList* child = create_list_item(object);
  child->next = root->head;
  if (NULL != root->head) {
    root->head->prev = child;
  }
  root->head = child;
}

void linked_list_insert(LinkedRoot* root, void* object, uint16_t after) {
  if (NULL == root) {
    return;
  }
  LinkedList* child = create_list_item(object);
  LinkedList* list = list_get(root, after);
  if (NULL == list) {
    linked_list_append(root, object);
  }
  else {
    child->next = list->next;
    list->next = child;
    child->prev = list;
  }
}

void* linked_list_get(LinkedRoot* root, uint16_t index) {
  if (NULL == root) {
    return NULL;
  }
  LinkedList* list = list_get(root, index);
  return NULL == list ? NULL : list->object;
}

void linked_list_remove(LinkedRoot* root, uint16_t index) {
  if (NULL == root) {
    return;
  }
  LinkedList* list = list_get(root, index);
  if (NULL == list) {
    return;
  }
  if (NULL == list->prev) {
    root->head = list->next;
    if (NULL != list->next) {
      list->next->prev = NULL;
    }
  }
  else {
    if (NULL != list->next) {
      list->next->prev = list->prev;
    }
    list->prev->next = list->next;
  }
}

void linked_list_clear(LinkedRoot* root) {
  if (NULL == root) {
    return;
  }
  while (root->head != NULL) {
    linked_list_remove(root, 0);
  }
}

bool linked_list_contains(LinkedRoot* root, void* object) {
  return linked_list_contains_compare(root, object, pointer_compare);
}

bool linked_list_contains_compare(LinkedRoot* root, void* object, ObjectCompare compare) {
  return linked_list_find_compare(root, object, compare) >= 0;
}

int16_t linked_list_find(LinkedRoot* root, void* object) {
  return linked_list_find_compare(root, object, pointer_compare);
}

int16_t linked_list_find_compare(LinkedRoot* root, void* object, ObjectCompare compare) {
  int16_t index = 0;
  LinkedList* list = root->head;
  while (list != NULL) {
    if (compare(object, list->object)) {
      return index;
    }
    list = list->next;
    index += 1;
  }
  return -1;
}

//----------------------------------------------------------------------------//

// Create a new LinkedList with the specified object.
static LinkedList* create_list_item(void* object) {
  LinkedList* item = malloc(sizeof(LinkedList));
  item->next = NULL;
  item->prev = NULL;
  item->object = object;
  return item;
}

// Returns the LinkedList at a given position within a list.
static LinkedList* list_get(LinkedRoot* root, uint16_t index) {
  LinkedList* list = root->head;
  uint16_t pos = 0;
  while (NULL != list) {
    if (index == pos) {
      return list;
    }
    list = list->next;
    pos += 1;
  }
  return NULL;
}

// ObjectCompare function used for comparing the pointers of two objects.
// Used by the find/contains functions that look for identical objects.
static bool pointer_compare(void* p1, void* p2) {
  return p1 == p2;
}