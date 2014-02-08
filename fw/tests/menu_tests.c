/** 
 * menu_tests.c 
 * 
 * This file contains tests for 
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define __STDC_VERSION__ 19901l
#include <greatest.h>

#include "../menu.c"
#include "lcd_drv_mock.h"
#include "../menu-defs.h"


TEST menu_doesNotChangeDisplayContents_whenNotActivated() {
	// fixtrure: initialise the menu
  menu_reset();

  // execution: do nothing and loop
  menu_loop();

  // assertion: should not have changed the display contents
  ASSERT_EQ(strncmp(LCD_CONTENTS, "", 1), 0);
  PASS();
}

TEST menu_printsPizzatimer1_whenButtonIsPressed() {
  // fixture: initialise the menu.
  menu_reset();

  // execution: press button and let the menu execute one loop iteration
  menu_activate();
  menu_loop();

  // assertion: should have printed Pizzatimer 1 in the display
  ASSERT_EQ( strncmp(LCD_CONTENTS, "Pizzatimer 1", sizeof("Pizzatimer 1")), 0);
  PASS();
}

TEST menu_printsPizzatimerN_whenDownbuttonIsPressedFromPizzatimer1Entry(int timesButtonDownBefore, const char* expectedString) {
	// fixture: activate pizza timer X 
  menu_reset();
  menu_activate();
  int ttt = timesButtonDownBefore;
  for (; timesButtonDownBefore--;) {
    menu_button_down();
  }

  // execution: press down button
  menu_button_down();
  menu_loop();

  // should print "Pizzatimer 2" now
  ASSERT_EQ( strncmp(LCD_CONTENTS, expectedString, strlen(expectedString)), 0);
  PASS();
}

TEST menu_printsPizzatimerN_whenUpButtonIsPressedFromPizzatimer1Entry(int timesButtonDownBefore, const char* expectedString) {
	// fixture: activate pizza timer X 
  menu_reset();
  menu_activate();
  for (; timesButtonDownBefore--;) {
    menu_button_down();
  }

  // execution: press down button
  menu_button_up();
  menu_loop();

  // should print "Pizzatimer 2" now
  ASSERT_EQ( strncmp(LCD_CONTENTS, expectedString, strlen(expectedString)), 0);
  PASS();
}

TEST menu_selectsPizzaTimerN_whenButtonIsPressedInMenu(int timesButtonDownBefore, int targetMenuState, int selectedKey) {
  extern int menu_state, selected_key;

  // fixture: make timer n the current menu entry
  menu_reset();
  menu_activate();
  for (; timesButtonDownBefore--;) {
    menu_button_down();
  }

  // execution: press the button
  ASSERT_EQ(menu_state, targetMenuState);
  menu_activate();

  // assertion: should have triggered some action on the selected timer.
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_REPAINT);
  ASSERT_EQ(selected_key, selectedKey);
  PASS();
}

TEST menu_addsOneSecond_whenButtonUpIsPressed() {
  extern int menu_state, selected_key, selected_time;

  // fixture: go to the pizzatimer 1 - time selection.
  menu_reset();
  menu_activate();
  menu_activate();

  // execution: press down button
  menu_button_up();
  menu_loop();

  // assertion: should be in time selection mode for pizzatimer and have a time of 301 seconds selected
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  ASSERT_EQ(selected_key, KEY_ID_PIZZATIMER_1);
  ASSERT_EQ(selected_time, 360);
  ASSERT_EQ(strcmp(LCD_CONTENTS, "Pizzatimer 1    06:00"), 0);
}

TEST menu_subtractsOneSecond_whenButtonDownIsPressed() {
  extern int menu_state, selected_key, selected_time;

  // fixture: go to the pizzatimer 1 - time selection.
  menu_reset();
  menu_activate();
  menu_activate();

  // execution: press down button
  menu_button_down();
  menu_loop();

  // assertion: should be in time selection mode for pizzatimer and have a time of 301 seconds selected
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  ASSERT_EQ(selected_key, KEY_ID_PIZZATIMER_1);
  ASSERT_EQ(selected_time, 240);
  ASSERT_EQ(strcmp(LCD_CONTENTS, "Pizzatimer 1    04:00"), 0);
}

TEST menu_capsSeconds_whenButtonIsUpAndTimeIs99Minutes() {
  extern int menu_state, selected_key, selected_time;
  int x;

  // fixture: go to the pizzatimer 1 - time selection.
  menu_reset();
  menu_activate();
  menu_activate();

  // execution: press down button
  for (x = 0; x < 100; x++) {
    menu_button_up();
  }
  menu_loop();

  // assertion: should be in time selection mode for pizzatimer and have a time of 301 seconds selected
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  ASSERT_EQ(selected_key, KEY_ID_PIZZATIMER_1);
  ASSERT_EQ(selected_time, 5940);
  ASSERT_EQ(strcmp(LCD_CONTENTS, "Pizzatimer 1    99:00"), 0);
}

TEST menu_capsSeconds_whenButtonIsDownAndTimeIs0Minutes() {
  extern int menu_state, selected_key, selected_time;
  int x;

  // fixture: go to the pizzatimer 1 - time selection.
  menu_reset();
  menu_activate();
  menu_activate();

  // execution: press down button
  for (x = 0; x < 100; x++) {
    menu_button_down();
  }
  menu_loop();

  // assertion: should be in time selection mode for pizzatimer and have a time of 301 seconds selected
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  ASSERT_EQ(selected_key, KEY_ID_PIZZATIMER_1);
  ASSERT_EQ(selected_time, 60);
  ASSERT_EQ(strcmp(LCD_CONTENTS, "Pizzatimer 1    01:00"), 0);
}

TEST menu_goesAway_whenTimeoutOccurs() {
  extern int menu_state, menu_timer;
  extern void menutimer();

  // fixture: go to pizzatimer 1 menu
  menu_reset();
  menu_activate();

  // execution: wait for $TIME
  int i;
  for (i = 0; i < MENU_TIMEOUT_SECONDS-1; i++) {
    menutimer();
    ASSERT_EQ(menu_state, MENU_STATE_PIZZA1);
  }
  menutimer();

  // assertion: after MENU_TIMEOUT_SECONDS seconds the timer should have triggered exit of the menu.
  ASSERT_EQ(menu_state, MENU_STATE_INACTIVE);
  PASS();
}

TEST menu_goesAway_whenInTimeSelectionMode() {
  extern int menu_state, menu_timer;
  extern void menutimer();

  // fixture:
  menu_reset();
  menu_activate();
  menu_activate();
  menu_loop();

  // execution:
  int i;
  for (i = 0; i < MENU_TIMEOUT_SELECT_SECONDS - 1; i++) {
    menutimer();
    ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  }
  menutimer();

  // assertion: should be in inactive mode now. timer should have been called.
  ASSERT_EQ(menu_state, MENU_STATE_INACTIVE);
  PASS();
}

TEST menu_refreshesTimer_whenInputUpIsTriggered() {
  extern int menu_state, menu_timer;
  extern void menutimer();

  // fixture: reset and activate the menu
  menu_reset();
  menu_activate();
  menu_loop();

  // execution: perform input
  int i;
  for (i = 0; i < 10; i++) {
    menutimer();
  }
  menu_button_down();

  // assertion: menu timer should be reset
  ASSERT_EQ(menu_timer, MENU_TIMEOUT_SECONDS);
  PASS();
}

TEST menu_refreshesTimer_whenInputDownIsTriggered() {
  extern int menu_state, menu_timer;
  extern void menutimer();

  // fixture: reset and activate the menu
  menu_reset();
  menu_activate();
  menu_loop();

  // execution: perform input
  int i;
  for (i = 0; i < 10; i++) {
    menutimer();
  }
  menu_button_up();

  // assertion: menu timer should be reset
  ASSERT_EQ(menu_timer, MENU_TIMEOUT_SECONDS);
  PASS();
}




SUITE (menu_functionality) {
  RUN_TEST(menu_doesNotChangeDisplayContents_whenNotActivated);

  // Menu akivieren
	RUN_TEST(menu_printsPizzatimer1_whenButtonIsPressed);

  // Button nach unten
  RUN_TESTp(menu_printsPizzatimerN_whenDownbuttonIsPressedFromPizzatimer1Entry, 0, "Pizzatimer 2");
  RUN_TESTp(menu_printsPizzatimerN_whenDownbuttonIsPressedFromPizzatimer1Entry, 1, "Pizzatimer 3");
  RUN_TESTp(menu_printsPizzatimerN_whenDownbuttonIsPressedFromPizzatimer1Entry, 2, "Pizzatimer 1");
  
  // Button nach oben
  RUN_TESTp(menu_printsPizzatimerN_whenUpButtonIsPressedFromPizzatimer1Entry, 0, "Pizzatimer 3");
  RUN_TESTp(menu_printsPizzatimerN_whenUpButtonIsPressedFromPizzatimer1Entry, 1, "Pizzatimer 1");
  RUN_TESTp(menu_printsPizzatimerN_whenUpButtonIsPressedFromPizzatimer1Entry, 2, "Pizzatimer 2");

  // timer auswählen
  RUN_TESTp(menu_selectsPizzaTimerN_whenButtonIsPressedInMenu, 0, MENU_STATE_PIZZA1, KEY_ID_PIZZATIMER_1);
  RUN_TESTp(menu_selectsPizzaTimerN_whenButtonIsPressedInMenu, 1, MENU_STATE_PIZZA2, KEY_ID_PIZZATIMER_2);
  RUN_TESTp(menu_selectsPizzaTimerN_whenButtonIsPressedInMenu, 2, MENU_STATE_PIZZA3, KEY_ID_PIZZATIMER_3);

  // zeit einstellen fuer irgendwas
  RUN_TEST(menu_addsOneSecond_whenButtonUpIsPressed);  
  RUN_TEST(menu_subtractsOneSecond_whenButtonDownIsPressed);  
  RUN_TEST(menu_capsSeconds_whenButtonIsUpAndTimeIs99Minutes);
  RUN_TEST(menu_capsSeconds_whenButtonIsDownAndTimeIs0Minutes);

  // timeout -> menu beendet
  RUN_TEST(menu_goesAway_whenTimeoutOccurs);

  // timeout -> laenge beendet
  RUN_TEST(menu_goesAway_whenInTimeSelectionMode);

  // timer refresh -> any input.
  RUN_TEST(menu_refreshesTimer_whenInputUpIsTriggered);
  RUN_TEST(menu_refreshesTimer_whenInputDownIsTriggered);
}


/** stuff that needs to be in the main file */
GREATEST_MAIN_DEFS();

int main(int argc, char** argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(menu_functionality);
    GREATEST_MAIN_END();        /* display results */

  return 0;
}


