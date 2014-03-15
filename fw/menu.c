
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "menu-defs.h"
#include "common.h"


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
    case MENU_STATE_PIZZA1:   if (pizzatimer_running(0) != 0) {
                                pizzatimer_clear(0);
                                menu_state = MENU_STATE_INACTIVE;
                              } else { 
                                selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                                selected_key = KEY_ID_PIZZATIMER_1; 
                                menu_state = MENU_STATE_SELECT_REPAINT; 
                              }
                              break;
    case MENU_STATE_PIZZA2:   if (pizzatimer_running(1) != 0) {
                                pizzatimer_clear(1);
                                menu_state = MENU_STATE_INACTIVE;
                              } else { 
                                selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                                selected_key = KEY_ID_PIZZATIMER_2; 
                                menu_state = MENU_STATE_SELECT_REPAINT; 
                              }
                              break;
    case MENU_STATE_PIZZA3:   if (pizzatimer_running(2) != 0) { 
                                pizzatimer_clear(2);
                                menu_state = MENU_STATE_INACTIVE;
                              } else { 
                                selected_time = PIZZA_TIMER_DEFAULT_TIME; 
                                selected_key = KEY_ID_PIZZATIMER_3; 
                                menu_state = MENU_STATE_SELECT_REPAINT; 
                              }
                              break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME:
                              menu_state = MENU_STATE_APPLY_TIMER;
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
  } else if (menu_state == MENU_STATE_APPLY_TIMER) { 
    setKeyTimeout(selected_key, selected_time);
    menu_state = MENU_STATE_INACTIVE;
    return;
  }

  switch (menu_state) {
	  case MENU_STATE_PIZZA1:      if (pizzatimer_running(0) != 0) { 
                                   menu_print("Pizzatimer 1 Off");
                                 } else {
                                   menu_print("Pizzatimer 1");	
                                 }
                                 break;

	  case MENU_STATE_PIZZA2:      if (pizzatimer_running(1) != 0) { 
                                   menu_print("Pizzatimer 2 Off");
                                 } else {
                                   menu_print("Pizzatimer 2");
                                 } 
                                 break;

	  case MENU_STATE_PIZZA3:      if (pizzatimer_running(2) != 0) { 
                                   menu_print("Pizzatimer 3 Off");
                                 } else { 
                                   menu_print("Pizzatimer 3");
                                 } 
                                 break;

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
    if (menu_state == MENU_STATE_SELECT_TIME || menu_state == MENU_STATE_SELECT_REPAINT) {
      menu_state = MENU_STATE_APPLY_TIMER;
    } else {
      menu_state = MENU_STATE_INACTIVE;
    }
  }
}


#ifndef __NO_INCLUDE_AVR

int call_menu() {
  extern void initTimers();
	extern void key_timer();
  extern void key_smaul();
	extern void keytimer_displayupdat();
  extern void keytimer_displayupdate();

  initTimers();

  for (;;) {

		for (;;) {
			keytimer_displayupdate();
			switch(get_event()) {
				case EV_ENCODER_PUSH: goto startmenu;
				case EV_TICK:			    key_timer(); 
                              break;
        case EV_SMAUL_PUSH:   key_smaul();
                              break;
			}
		}

startmenu:
	  menu_reset();
	  menu_activate();
	
	  for (;menu_state != MENU_STATE_INACTIVE;) {
	    menu_loop();
	
	    switch(get_event()) {
	      case EV_ENCODER_CCW:     menu_button_up(); break;
	      case EV_ENCODER_CW:      menu_button_down(); break;
	      case EV_ENCODER_PUSH:    menu_activate(); break;
	      case EV_TICK:            menutimer(); break;
	      case EV_SMAUL_PUSH:      break;
	    }
	  }

	}
}

#endif // __NO_INCLUDE_AVR


