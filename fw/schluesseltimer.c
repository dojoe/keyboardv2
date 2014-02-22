
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "common.h"
#include "schluesseltimer.h"

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

void schluessel_timer() {
  triggerDisplayUpdate = 1;

  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] > 0) {
      keyTimers[i] --;
    }
  }

}

void setKeyTimeout(int key, int time) { 
  if (key >= KEY_ID_PIZZATIMER_OFFSET) {
    keyTimers[MAX_KEYS+(key-KEY_ID_PIZZATIMER_OFFSET)] = time;
  } 
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
    sprintf(buffer, "%2dm ", timeInSeconds);
  } else {
    // print <timeInSeconds / 60>m
    sprintf(buffer, "%2ds ", timeInSeconds / 60);
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

void schluesseltimer_displayupdate() {
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



