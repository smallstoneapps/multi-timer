/*

Linked List v0.2.3
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

typedef struct LinkedList LinkedList;
typedef struct LinkedRoot LinkedRoot;
typedef bool (*ObjectCompare)(void* object1, void* object2);

LinkedRoot* linked_list_create_root(void);
uint16_t linked_list_count(LinkedRoot* root);
void linked_list_append(LinkedRoot* root, void* object);
void linked_list_prepend(LinkedRoot* root, void* object);
void linked_list_insert(LinkedRoot* root, void* object, uint16_t after);
void* linked_list_get(LinkedRoot* root, uint16_t index);
void linked_list_remove(LinkedRoot* root, uint16_t index);
void linked_list_clear(LinkedRoot* root);
bool linked_list_contains(LinkedRoot* root, void* object);
bool linked_list_contains_compare(LinkedRoot* root, void* object, ObjectCompare compare);
int16_t linked_list_find(LinkedRoot* root, void* object);
int16_t linked_list_find_compare(LinkedRoot* root, void* object, ObjectCompare compare);