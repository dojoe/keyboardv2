/** 
 * pizzatimer_tests.c 
 * 
 * This file contains tests for the pizza timer state machine
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define __STDC_VERSION__ 19901l
#include <greatest.h>

#include "beeper_mock.h"
#include "../schluesseltimer.c"
#include "lcd_drv_mock.h"


TEST timer_doesNotBeep_whenTimersAreDeactivated() {
	// fixtrure:
  initTimers(); 

  // execution: call the pizzatimer
  schluessel_timer();

  // assertion:
  PASS();
}

TEST schluesseltimerDisplayupte_displaysNothing_whenTimersAreDeactivated() {
  // fixture:
  initTimers(); 

  // execution: call the update method
  schluesseltimer_displayupdate();

  // assertion: should have cleared the screen.
  ASSERT_EQ(strncmp(LCD_CONTENTS, "", 1), 0);
  PASS();
}

TEST schluesseltimer_displaysTime_whenPizzaTimerIsActive() {
  // fixture: set to idle status make a pizza timer be enabled
  initTimers(); 
  keyTimers[MAX_KEYS] = 5 * 60 +  + 5;

  // execution: call the display-update method for the timer
  schluessel_timer(); 
  schluesseltimer_displayupdate();

  // assertion: should print the state of the first pizza timer on the display.
  ASSERT_EQ(strncmp(LCD_CONTENTS, "                 5m        ", 18), 0);
  PASS();
}

TEST setKeyTimeout_updatesInternalState(int keyId, int slotId) {
  extern int keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];

  // fixture: init timers
  initTimers();

  // execution: set timer
  setKeyTimeout(keyId, 300);

  // assertion: should be written to the given slot.
  ASSERT_EQ(keyTimers[slotId], 300);
  PASS();
}

TEST schluesseltimer_displaysTime_andUpdatesAfterTimerEvent() {
  extern int keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];

  // fixture: set to idle status to make pizza timer enabled
  initTimers();
  schluessel_timer();
  setKeyTimeout(KEY_ID_PIZZATIMER_1, 4 * 60);

  // assertion + execution: display value, count down and display again.
  schluesseltimer_displayupdate();
  ASSERT_EQ(strncmp(LCD_CONTENTS, "                 4m        ", 18), 0);
  schluessel_timer();
  schluesseltimer_displayupdate();
  ASSERT_EQ(strncmp(LCD_CONTENTS, "                 3m        ", 18), 0);
  PASS();
}

TEST schluesseltimer_beeps_whenTimerRunsOut(int key, int sound) {
  // fixture: set to idle status to make pizza timer enabled
  initTimers();
  schluessel_timer();
  resetBeeper();
  setKeyTimeout(key, 1);

  // assertion + execution: display value, count down and display again.
  schluessel_timer();
	ASSERT_EQ(beeper_sound, sound);
  PASS();
}






SUITE (timer_functionality) {
	// if no timer is active.
  RUN_TEST(timer_doesNotBeep_whenTimersAreDeactivated);
  RUN_TEST(schluesseltimerDisplayupte_displaysNothing_whenTimersAreDeactivated);

  // activation of timers
  RUN_TESTp(setKeyTimeout_updatesInternalState, KEY_ID_PIZZATIMER_1, MAX_KEYS+0);
  RUN_TESTp(setKeyTimeout_updatesInternalState, KEY_ID_PIZZATIMER_2, MAX_KEYS+1);
  RUN_TESTp(setKeyTimeout_updatesInternalState, KEY_ID_PIZZATIMER_3, MAX_KEYS+2);

  // if pizza-timer is active.
	RUN_TEST(schluesseltimer_displaysTime_whenPizzaTimerIsActive);
  RUN_TEST(schluesseltimer_displaysTime_andUpdatesAfterTimerEvent);

  // if a timer runs out
  RUN_TESTp(schluesseltimer_beeps_whenTimerRunsOut, KEY_ID_PIZZATIMER_1, BEEP_PIZZA1);
  RUN_TESTp(schluesseltimer_beeps_whenTimerRunsOut, KEY_ID_PIZZATIMER_2, BEEP_PIZZA2);
  RUN_TESTp(schluesseltimer_beeps_whenTimerRunsOut, KEY_ID_PIZZATIMER_3, BEEP_PIZZA3);
  RUN_TESTp(schluesseltimer_beeps_whenTimerRunsOut, 1, BEEP_KEYMISSING);	
}


/** stuff that needs to be in the main file */
GREATEST_MAIN_DEFS();

int main(int argc, char** argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(timer_functionality);
    GREATEST_MAIN_END();        /* display results */

  return 0;
}


