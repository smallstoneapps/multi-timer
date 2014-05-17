/***
 * Message Queue
 * Copyright © 2013 Matthew Tole
 *
 * Version 1.0.0
 ***/

#pragma once

typedef void (*MessageHandler)(char* operation, char* data);

void mqueue_init(void);
bool mqueue_add(char* group, char* operation, char* params);
void mqueue_register_handler(char* group, MessageHandler handler);