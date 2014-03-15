
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "menu-defs.h"
#include "common.h"
#include "panel.h"
#include "ui.h"

enum ui_state {
	UIS_IDLE,
	UIS_MENU,
};

uint8_t  menu_state = 0;
uint8_t  selected_key = 0;
uint16_t selected_time = 0;
uint8_t  menu_timer = 0;
uint8_t  ui_state = UIS_IDLE;

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

static void menu_repaint() {
	uint8_t n;

	switch (menu_state) {
	case MENU_STATE_PIZZA1:
	case MENU_STATE_PIZZA2:
	case MENU_STATE_PIZZA3:
		n = menu_state - MENU_STATE_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			lcd_printfP(0, PSTR("Pizzatimer %d Off"), n + 1);
		} else {
			lcd_printfP(0, PSTR("Pizzatimer %d"), n + 1);
		}
		break;

	case MENU_STATE_SELECT_REPAINT:
	case MENU_STATE_SELECT_TIME:
		lcd_printfP(1, PSTR("%02i:%02i"), selected_time / 60,
				selected_time % 60);
		break;
	}

}

void menu_enter(void) {
	menu_state = MENU_STATE_PIZZA1;
	ui_state = UIS_MENU;
	_resetTimer();
	lcd_printfP(0, PSTR(""));
	lcd_printfP(1, PSTR(""));
}

void menu_exit(void) {
	menu_state = MENU_STATE_INACTIVE;
	ui_state = UIS_IDLE;
	lcd_printfP(0, PSTR(""));
	lcd_printfP(1, PSTR(""));
}

static void menu_loop() {
	uint8_t n;

  if (menu_state == MENU_STATE_INACTIVE) {
    return;
  } else if (menu_state == MENU_STATE_APPLY_TIMER) { 
    setKeyTimeout(selected_key, selected_time);
    menu_exit();
    return;
  }

  switch (menu_state) {
	  case MENU_STATE_SELECT_REPAINT:
		  menu_state = MENU_STATE_SELECT_TIME;
		  break;
  }
}

/**
 * this method is externally called when the
 */
void menu_activate() {
	uint8_t n;

	switch (menu_state) {
	// enable the menu;
	case MENU_STATE_INACTIVE:
		menu_state = MENU_STATE_PIZZA1;
		break;

	case MENU_STATE_PIZZA1:
	case MENU_STATE_PIZZA2:
	case MENU_STATE_PIZZA3:
		n = menu_state - MENU_STATE_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			pizzatimer_clear(n);
			menu_exit();
		} else {
			selected_time = PIZZA_TIMER_DEFAULT_TIME;
			selected_key = KEY_ID_PIZZATIMER_OFFSET + n;
			menu_state = MENU_STATE_SELECT_REPAINT;
		}
		break;

	case MENU_STATE_SELECT_REPAINT:
	case MENU_STATE_SELECT_TIME:
		menu_state = MENU_STATE_APPLY_TIMER;
		break;
	}

	_resetTimer();
	menu_repaint();
}

static void menu_button_down() {
  _resetTimer();
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		 	 menu_state = MENU_STATE_PIZZA2; break;
    case MENU_STATE_PIZZA2:		   menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA3:		   menu_state = MENU_STATE_PIZZA1; break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME: menu_state = MENU_STATE_SELECT_REPAINT;
                                 selected_time = max(60, selected_time+60);
                                 break;
  }

  menu_repaint();
}

static void menu_button_up() {
  _resetTimer();
  switch(menu_state) {
    case MENU_STATE_PIZZA1:		   menu_state = MENU_STATE_PIZZA3; break;
    case MENU_STATE_PIZZA2:		   menu_state = MENU_STATE_PIZZA1; break;
    case MENU_STATE_PIZZA3:		   menu_state = MENU_STATE_PIZZA2; break;

    case MENU_STATE_SELECT_REPAINT:
    case MENU_STATE_SELECT_TIME: menu_state = MENU_STATE_SELECT_REPAINT;
                                 selected_time = min(99*60, selected_time-60);
                                 break;
  }

  menu_repaint();
}

void menutimer() {
  menu_timer--;
  if (menu_timer < 1) {
    if (menu_state == MENU_STATE_SELECT_TIME || menu_state == MENU_STATE_SELECT_REPAINT) {
      menu_state = MENU_STATE_APPLY_TIMER;
    } else {
      menu_exit();
    }
  }
}


#ifndef __NO_INCLUDE_AVR

static void ui_poll_idle(void)
{
	keytimer_displayupdate();

	switch (get_event()) {
	case EV_ENCODER_PUSH:
		enable_lcd_backlight();
		menu_enter();
		menu_repaint();
		break;
	case EV_TICK:
		key_timer();
		break;
	case EV_SMAUL_PUSH:
		key_smaul();
		break;
	case EV_ENCODER_CCW:
	case EV_ENCODER_CW:
		enable_lcd_backlight();
		break;
	}
}

static void ui_poll_menu(void)
{
	switch (get_event()) {
	case EV_ENCODER_CCW:
		enable_lcd_backlight();
		menu_button_up();
		break;
	case EV_ENCODER_CW:
		enable_lcd_backlight();
		menu_button_down();
		break;
	case EV_ENCODER_PUSH:
		enable_lcd_backlight();
		menu_activate();
		break;
	case EV_TICK:
		key_timer();
		menutimer();
		break;
	case EV_SMAUL_PUSH:
		menu_exit();
		break;
	}

	menu_loop();
}

void ui_init(void)
{
	initTimers();
}

void ui_poll(void)
{
	switch (ui_state) {
	case UIS_IDLE:
		ui_poll_idle();
		break;
	case UIS_MENU:
		ui_poll_menu();
		break;
	}
}

#endif // __NO_INCLUDE_AVR


