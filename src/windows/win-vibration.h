#pragma once

#include "../timer.h"

typedef void (*VibrationCallback)(TimerVibration vibration);

void win_vibration_init(void);
void win_vibration_show(VibrationCallback callback, TimerVibration vibration);