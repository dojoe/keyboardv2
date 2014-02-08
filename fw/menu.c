
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "menu-defs.h"


int menu_state = 0;
int selected_key = 0;
int selected_time = 0;
int menu_timer = 0;

void menu_reset() {
  menu_state = MENU_STATE_INACTIVE;
  menu_timer = 0;
}


/** 
 * this method is used internally to reset the timer for ending the menu mode.
 */
void _resetTimer() {
  switch(menu_state) {
    case MENU_STATE_INACTIVE:  menu_timer = 0; break;

    case MENU_STATE_PIZZA1:
    case MENU_STATE_PIZZA2:
    case MENU_STATE_PIZZA3:    menu_timer = MENU_TIMEOUT_SECONDS; break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME:
                               menu_timer = MENU_TIMEOUT_SELECT_SECONDS; break;
  }
}

/** 
 * this method is externally called when the 
 */
void menu_activate() {
  menu_timer =  MENU_TIMEOUT_SECONDS;
  switch (menu_state) {
    // enable the menu;
    case MENU_STATE_INACTIVE: menu_state = MENU_STATE_PIZZA1; 
                              break;
    case MENU_STATE_PIZZA1:   selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                              selected_key = KEY_ID_PIZZATIMER_1; 
                              menu_state = MENU_STATE_SELECT_REPAINT; 
                              break;
    case MENU_STATE_PIZZA2:   selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                              selected_key = KEY_ID_PIZZATIMER_2; 
                              menu_state = MENU_STATE_SELECT_REPAINT; 
                              break;
    case MENU_STATE_PIZZA3:   selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                              selected_key = KEY_ID_PIZZATIMER_3; 
                              menu_state = MENU_STATE_SELECT_REPAINT; 
                              break;
  }
                              
  _resetTimer();
}

static void menu_print(const char* s) {
  lcd_blank(32);
  lcd_xy(0, 0);

  lcd_puts((char*) s);
}


void menu_loop() {
  char buffer[64]; 

  if (menu_state == MENU_STATE_INACTIVE) {
    return;
  }

  switch (menu_state) {
	  case MENU_STATE_PIZZA1:      menu_print("Pizzatimer 1");	break;
	  case MENU_STATE_PIZZA2:      menu_print("Pizzatimer 2");	break;
	  case MENU_STATE_PIZZA3:      menu_print("Pizzatimer 3");	break;

    case MENU_STATE_SELECT_REPAINT:
                                 sprintf(buffer, "Pizzatimer 1    %02i:%02i", selected_time / 60, selected_time % 60);
                                 menu_print(buffer); 
                                 menu_state = MENU_STATE_SELECT_TIME;
                                 break;
  }
}

void menu_button_down() {
  _resetTimer();
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		 	 menu_state = MENU_STATE_PIZZA2; break;
    case MENU_STATE_PIZZA2:		   menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA3:		   menu_state = MENU_STATE_PIZZA1; break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME: menu_state = MENU_STATE_SELECT_REPAINT;
                                 selected_time = max(60, selected_time-60);
                                 break;
  }
}

void menu_button_up() {
  _resetTimer();
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		   menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA2:		   menu_state = MENU_STATE_PIZZA1; break;
    case MENU_STATE_PIZZA3:		   menu_state = MENU_STATE_PIZZA2; break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME: menu_state = MENU_STATE_SELECT_REPAINT;
                                 selected_time = min(99*60, selected_time+60);
                                 break;
  }
}

void menutimer() {
  menu_timer--;
  if (menu_timer < 1) {
    menu_state = MENU_STATE_INACTIVE;
    menu_timer = 0;
  }
}


#ifndef __NO_INCLUDE_AVR

int call_menu() {
  menu_reset();
  menu_activate();

  for (;;) {
    menu_loop();

    switch(next_input_event()) {
      case IN_ENCODER_CCW:   menu_button_up(); break;
      case IN_ENCODER_CW:  menu_button_down(); break;
      case IN_ENCODER_PUSH: menu_activate(); break;
      case IN_SMAUL_PUSH:   return;
    }
  }
}

#endif // __NO_INCLUDE_AVR


