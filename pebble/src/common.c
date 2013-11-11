/***
 * Multi Timer
 * Copyright © 2013 Matthew Tole
 *
 * common.c
 ***/

#include "libs/xprintf.h"

void uppercase(char* str) {
  char* point = str;
  while (*point != '\0') {
    if (*point >= 97 && *point <= 122) {
      *point -= 32;
    }
    point += 1;
  }
}

int patoi(char* str) {
  long num;
  xatoi(&str, &num);
  return (int)num;
}