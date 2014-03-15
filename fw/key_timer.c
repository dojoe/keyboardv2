
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "common.h"
#include "key_timer.h"

// the key-timers.
int timer_state = 0;
int keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];
static int triggerDisplayUpdate = 1;

void initTimers() {
  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    keyTimers[i] = -1;
  }
}


static void menu_start_beeping(int slot) {
  if (slot == MAX_KEYS + 0) {
    beeper_start(BEEP_PIZZA1);
  } else if (slot == MAX_KEYS + 1) {
    beeper_start(BEEP_PIZZA2);
  } else if (slot == MAX_KEYS + 2) {
    beeper_start(BEEP_PIZZA3);
  } else {
    beeper_start(BEEP_KEYMISSING);
  }
}

void key_smaul() {
  int i;
  // is there a key missing?
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] == 0) {
      // clear the first key timer -> set + 5 Minutes.
      keyTimers[i] = 300;
      break;
    }
  }

  // check if there's another key that needs handling.
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] == 0) {
      menu_start_beeping(i);
      return;
    }
  }

  beeper_start(BEEP_OFF);
}

void key_timer() {
  triggerDisplayUpdate = 1;

  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] > 0) {
      keyTimers[i] --;
      if (keyTimers[i] == 0) {
        menu_start_beeping(i);
      }
    }
  }

}

void setKeyTimeout(int key, int time) { 
  if (key >= KEY_ID_PIZZATIMER_OFFSET) {
    keyTimers[MAX_KEYS+(key-KEY_ID_PIZZATIMER_OFFSET)] = time;
    return;
  } 

  // TODO: find out which key is missing and 
}


static void print_time(int timeInSeconds) {
  if (timeInSeconds == -1) {
    lcd_puts("    ");
    return;
  }

  char buffer[6];
  memset(buffer, 0, 6);

  if (timeInSeconds < 60) {
    // print <timeInSeconds>s
    sprintf(buffer, "%2ds ", timeInSeconds);
  } else {
    // print <timeInSeconds / 60>m
    sprintf(buffer, "%2dm ", timeInSeconds / 60);
  }

  lcd_puts(buffer);
}

/** 
 * helper to check if any timer is active
 * @return bool 0 or 1
 */
static int isTimerActive() {
  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] != -1) {
      return 1;
    }
  }

  return 0;
}

/** 
 * helper to check if a pizza timer is running
 * @param n pizza timer number, must be between 0 and MAX_KEYS
 */
int pizzatimer_running(int n) {
  if (n < 0 || n >= NUM_PIZZA_TIMERS) {
    return 0;
  }

  return keyTimers[MAX_KEYS + n] != -1;
}

/** 
 * helper to clear a pizza timer.
 */
void pizzatimer_clear(int n) {
  if (n >= 0 && n < NUM_PIZZA_TIMERS) {
    keyTimers[MAX_KEYS + n] = -1;
  }
}


/**  
 * helper to check what time the minimum key has, does not include pizza timers.
 */
static int getMinimumKeyTimer() {
  int i;
  for (i = 0; i < MAX_KEYS; i++) {
    if (keyTimers[i] != -1) {
      return keyTimers[i];
    }
  }

  return -1;
}

void keytimer_displayupdate() {
  if (! triggerDisplayUpdate) {
    return;
  }
  
  triggerDisplayUpdate = 0;

  lcd_blank(32);
  lcd_xy(0, 0);

  // if there's a key pulled, display in line 2...
  if (isTimerActive()) {
    lcd_puts("                ");

    // check pizza timers
    print_time(keyTimers[MAX_KEYS + 0]);
    print_time(keyTimers[MAX_KEYS + 1]);
    print_time(keyTimers[MAX_KEYS + 2]);

    // check other timer:
    if (getMinimumKeyTimer() != -1) {

    }
  }
}



