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

#pragma once

#include <pebble.h>

typedef struct LinkedRoot LinkedRoot;
typedef bool (*ObjectCompare)(void* object1, void* object2);

// Create a new LinkedRoot object, upon which all other linked list operations
// are performed.
LinkedRoot* linked_list_create_root(void);

// Returns the number of items currently in a linked list.
uint16_t linked_list_count(LinkedRoot* root);

// Adds a new item to the end of the linked list.
// This function does not copy the memory for the object, so you should ensure
// that the object does not get destroyed.
void linked_list_append(LinkedRoot* root, void* object);

// Adds a new item to the front of the linked list.
// This function does not copy the memory for the object, so you should ensure
// that the object does not get destroyed.
void linked_list_prepend(LinkedRoot* root, void* object);

// Adds a new item after the specified position in the linked list.
// This function does not copy the memory for the object, so you should ensure
// that the object does not get destroyed.
void linked_list_insert(LinkedRoot* root, void* object, uint16_t after);

// Returns the object at the specified position in the linked list.
void* linked_list_get(LinkedRoot* root, uint16_t index);

// Removes the object at the specified position in the list.
// This does not free the memory for the object, you should take care of that
// yourself.
void linked_list_remove(LinkedRoot* root, uint16_t index);

// Removes all items in a list.
// This does not free the memory for the objects in the list, you should take
// care of that yourself.
void linked_list_clear(LinkedRoot* root);

// Returns true if the specified object is in the list, otherwise returns false.
// Compares the pointers so this will only work with the exact same object, but
// not a copy.
bool linked_list_contains(LinkedRoot* root, void* object);

// Returns true if the specified object is in the list, otherwise returns false.
// Uses the specified ObjectCompare function to compare objects.
bool linked_list_contains_compare(LinkedRoot* root, void* object, ObjectCompare compare);

// Returns the index of the specified object in the list. If the object can not
// be found, returns -1.
// Compares the pointers so this will only work with the exact same object, but
// not a copy.
int16_t linked_list_find(LinkedRoot* root, void* object);

// Returns the index of the specified object in the list. If the object can not
// be found, returns -1.
// Uses the specified ObjectCompare function to compare objects.
int16_t linked_list_find_compare(LinkedRoot* root, void* object, ObjectCompare compare);
