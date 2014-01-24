

#include "hw.h"
#include "menu-defs.h"



static int menu_state = 0;

void menu_reset() {
  menu_state = MENU_STATE_INACTIVE;
}

void menu_activate() {
  menu_state = MENU_STATE_PIZZA1;
}

void menu_loop() {
  lcd_puts("Pizzatimer 1");
}



