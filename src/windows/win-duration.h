#pragma once

#include "../timer.h"

typedef void (*DurationCallback)(uint32_t duration);

void win_duration_init(void);
void win_duration_show(uint16_t duration, DurationCallback callback);
