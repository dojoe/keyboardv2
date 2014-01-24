/** 
 * menu_tests.c 
 * 
 * This file contains tests for 
 */

#include <string.h>
#include <stdint.h>
#include <greatest.h>

#include "lcd_drv_mock.h"


TEST menu_doesNotChangeDisplayContents_whenNotActivated() {

}

TEST menu_printsPizzatimer1_whenButtonIsPressed() {
  // fixture: initialise the menu.
  menu_reset();

  // execution: press button and let the menu execute one loop iteration
  menu_activate();
  menu_loop();

  // should have printed Pizzatimer 1 in the display
  ASSERT_EQ( strncmp(LCD_CONTENTS, "Pizzatimer 1", sizeof("Pizzatimer 1")), 0);
}

TEST menu_printsPizzatimer2_whenDownbuttonIsPressedFromPizzatimer1Entry() {

}



SUITE (menu_functionality) {
	RUN_TEST(menu_printsPizzatimer1_whenButtonIsPressed);
  RUN_TEST(menu_printsPizzatimer2_whenDownbuttonIsPressedFromPizzatimer1Entry);
}


/** stuff that needs to be in the main file */
GREATEST_MAIN_DEFS();

int main(int argc, char** argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(menu_functionality);
    GREATEST_MAIN_END();        /* display results */

  return 0;
}


