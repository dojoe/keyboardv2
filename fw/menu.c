

#include "hw.h"
#include "menu-defs.h"



int menu_state = 0;
int selected_key = 0;

void menu_reset() {
  menu_state = MENU_STATE_INACTIVE;
}

void menu_activate() {
  switch (menu_state) {
    // enable the menu;
    case MENU_STATE_INACTIVE: menu_state = MENU_STATE_PIZZA1; break;
    case MENU_STATE_PIZZA1:   selected_key = KEY_ID_PIZZATIMER_1; menu_state = MENU_STATE_SELECT_TIME; break;
    case MENU_STATE_PIZZA2:   selected_key = KEY_ID_PIZZATIMER_2; menu_state = MENU_STATE_SELECT_TIME; break;
    case MENU_STATE_PIZZA3:   selected_key = KEY_ID_PIZZATIMER_3; menu_state = MENU_STATE_SELECT_TIME; break;
  }
}

void menu_loop() {
  if (menu_state == MENU_STATE_INACTIVE) {
    return;
  }

  lcd_blank();
  switch (menu_state) {
	  case MENU_STATE_PIZZA1:    lcd_puts("Pizzatimer 1");	return;
	  case MENU_STATE_PIZZA2:    lcd_puts("Pizzatimer 2");	return;
	  case MENU_STATE_PIZZA3:    lcd_puts("Pizzatimer 3");	return;
  }
}

void menu_button_down() {
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		 menu_state = MENU_STATE_PIZZA2; break;
    case MENU_STATE_PIZZA2:		 menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA3:		 menu_state = MENU_STATE_PIZZA1; break;
  }
}

void menu_button_up() {
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		 menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA2:		 menu_state = MENU_STATE_PIZZA1; break;
    case MENU_STATE_PIZZA3:		 menu_state = MENU_STATE_PIZZA2; break;
  }
}


