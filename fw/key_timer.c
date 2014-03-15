
#include <stdio.h>
#include <string.h>
#ifndef __NO_INCLUDE_AVR
#include <avr/pgmspace.h>
#else
#define PSTR(str) str
#endif
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

  beeper_stop();
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


static void print_time(int timeInSeconds, char *destination) {
  if (timeInSeconds == -1) {
    strcpy_P(destination, PSTR("---"));
 } else if (timeInSeconds < 60) {
    // print <timeInSeconds>s
    sprintf_P(destination, PSTR("%2ds"), timeInSeconds);
  } else {
    // print <timeInSeconds / 60>m
    sprintf_P(destination, PSTR("%2dm"), timeInSeconds / 60);
  }
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
	char parts[4][4];

  if (! triggerDisplayUpdate) {
    return;
  }
  
  triggerDisplayUpdate = 0;

  // check pizza timers
  print_time(keyTimers[MAX_KEYS + 0], parts[0]);
  print_time(keyTimers[MAX_KEYS + 1], parts[1]);
  print_time(keyTimers[MAX_KEYS + 2], parts[2]);
  print_time(getMinimumKeyTimer(), parts[3]);

  lcd_printfP(1, PSTR("%s %s %s %s"), parts[0], parts[1], parts[2], parts[3]);
}



