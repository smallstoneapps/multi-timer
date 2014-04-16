/***
 * Multi Timer
 * Copyright © 2013 - 2014 Matthew Tole
 *
 * common.c
 ***/

#include <pebble.h>

void uppercase(char* str) {
  char* point = str;
  while (*point != '\0') {
    if (*point >= 97 && *point <= 122) {
      *point -= 32;
    }
    point += 1;
  }
}

void timer_duration_str(int duration, bool showHours, char* str, int str_len) {
  int hours = duration / 3600;
  int minutes = (showHours ? (duration % 3600) : duration) / 60;
  int seconds = (showHours ? (duration % 3600) : duration) % 60;
  if (showHours) {
    snprintf(str, str_len, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  else {
    snprintf(str, str_len, "%02d:%02d", minutes, seconds);
  }
}