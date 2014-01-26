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
  ASSERT_EQ(menu_state, MENU_STATE_SELECT_TIME);
  ASSERT_EQ(selected_key, selectedKey);
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
  

  // timeout -> menu beendet


  // timeout -> laenge beendet
}


/** stuff that needs to be in the main file */
GREATEST_MAIN_DEFS();

int main(int argc, char** argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(menu_functionality);
    GREATEST_MAIN_END();        /* display results */

  return 0;
}


