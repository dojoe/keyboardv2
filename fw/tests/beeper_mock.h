
#include "../panel.h"

uint8_t beeper_sound = BEEP_OFF;

void resetBeeper() {
  beeper_sound = BEEP_OFF;
}

void beeper_start(uint8_t soundType) {
  beeper_sound = soundType;
}


