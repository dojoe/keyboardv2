
#include <stdio.h>
#include <string.h>
#include "hw.h"
#include "lcd_drv.h"
#include "common.h"
#include "key_timer.h"

// the key-timers.
int keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];
uint8_t expired_key = 0;

void initTimers() {
  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    keyTimers[i] = -1;
  }
}

static void key_timer_expired(int slot) {
	if (slot >= MAX_KEYS) {
		beeper_start(slot - MAX_KEYS + BEEP_PIZZA1);
	} else {
		beeper_start(BEEP_KEYMISSING);
  }

  smaul_pulse(500);
	expired_key = slot + 1;
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
      key_timer_expired(i);
      return;
    }
  }

  expired_key = 0;
  beeper_stop();
  smaul_stop();
}

void key_timer() {
  int i;
  for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
    if (keyTimers[i] > 0) {
      keyTimers[i] --;
      if (keyTimers[i] == 0) {
        key_timer_expired(i);
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
		lcd_print_update_P(1, PSTR("--- "));
	} else if (timeInSeconds < 60) {
		// print <timeInSeconds>s
		lcd_print_update_P(1, PSTR("%2ds "), timeInSeconds);
	} else {
		// print <timeInSeconds / 60>m
		lcd_print_update_P(1, PSTR("%2dm "), timeInSeconds / 60);
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

	lcd_print_start(1);
	print_time(keyTimers[MAX_KEYS + 0], parts[0]);
	print_time(keyTimers[MAX_KEYS + 1], parts[1]);
	print_time(keyTimers[MAX_KEYS + 2], parts[2]);
	print_time(getMinimumKeyTimer(), parts[3]);
	lcd_print_end(1);
}



