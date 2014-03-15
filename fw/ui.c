
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "menu-defs.h"
#include "common.h"
#include "panel.h"
#include "ui.h"

uint8_t  ui_state = UIS_INACTIVE;
uint8_t  selected_key = 0;
uint16_t selected_time = 0;
uint8_t  ui_timer = 0;

static void menu_repaint(void) {
	uint8_t n;

	switch (ui_state) {
	case UIS_INACTIVE:
		keytimer_displayupdate();
		break;

	case UIS_PIZZA1:
	case UIS_PIZZA2:
	case UIS_PIZZA3:
		n = ui_state - UIS_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			lcd_printfP(0, PSTR("Pizzatimer %d Off"), n + 1);
		} else {
			lcd_printfP(0, PSTR("Pizzatimer %d"), n + 1);
		}
		break;

	case UIS_SELECT_TIME:
		lcd_printfP(1, PSTR("%02i:%02i"), selected_time / 60,
				selected_time % 60);
		break;
	}

}

/**
 * this method is used internally to reset the timer for ending the menu mode.
 */
static void reset_ui_timer(void) {
	switch (ui_state) {
	case UIS_INACTIVE:
		ui_timer = 0;
		break;

	case UIS_PIZZA1:
	case UIS_PIZZA2:
	case UIS_PIZZA3:
		ui_timer = MENU_TIMEOUT_SECONDS;
		break;

	case UIS_SELECT_TIME:
		ui_timer = MENU_TIMEOUT_SELECT_SECONDS;
		break;
	}
}

static void menu_enter(void) {
	ui_state = UIS_PIZZA1;
	reset_ui_timer();
	lcd_printfP(0, PSTR(""));
	lcd_printfP(1, PSTR(""));
}

static void menu_exit(void) {
	ui_state = UIS_INACTIVE;
	lcd_printfP(0, PSTR(""));
	lcd_printfP(1, PSTR(""));
}

static void apply_timer(void) {
	setKeyTimeout(selected_key, selected_time);
	menu_exit();
}

static void count_ui_timer(void) {
	if (ui_timer && !(--ui_timer)) {
		if (ui_state == UIS_SELECT_TIME) {
			apply_timer();
		} else {
			menu_exit();
		}
	}
}

static void menu_activate(void) {
	uint8_t n;

	switch (ui_state) {
	// enable the menu;
	case UIS_INACTIVE:
		menu_enter();
		break;

	case UIS_PIZZA1:
	case UIS_PIZZA2:
	case UIS_PIZZA3:
		n = ui_state - UIS_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			pizzatimer_clear(n);
			menu_exit();
		} else {
			selected_time = PIZZA_TIMER_DEFAULT_TIME;
			selected_key = KEY_ID_PIZZATIMER_OFFSET + n;
			ui_state = UIS_SELECT_TIME;
		}
		break;

	case UIS_SELECT_TIME:
		apply_timer();
		break;
	}
}

static void menu_button_down(void) {
  switch(ui_state) {
    case UIS_PIZZA1:		 	 ui_state = UIS_PIZZA2; break;
    case UIS_PIZZA2:		   ui_state = UIS_PIZZA3; break;
    case UIS_PIZZA3:		   ui_state = UIS_PIZZA1; break;

    case UIS_SELECT_TIME: selected_time = max(60, selected_time+60);
                                 break;
  }
}

static void menu_button_up(void) {
  switch(ui_state) {
    case UIS_PIZZA1:		   ui_state = UIS_PIZZA3; break;
    case UIS_PIZZA2:		   ui_state = UIS_PIZZA1; break;
    case UIS_PIZZA3:		   ui_state = UIS_PIZZA2; break;

    case UIS_SELECT_TIME: selected_time = min(99*60, selected_time-60);
                                 break;
  }
}

static void menu_button_smaul(void) {
	switch (ui_state) {
	case UIS_INACTIVE:
		key_smaul();
		break;
	default:
		menu_exit();
		break;
	}
}

#ifndef __NO_INCLUDE_AVR

void ui_poll(void)
{
	uint8_t event = get_event();

	if (event == EV_NONE)
		return;

	switch (event) {
	case EV_ENCODER_CCW:
		menu_button_up();
		break;
	case EV_ENCODER_CW:
		menu_button_down();
		break;
	case EV_ENCODER_PUSH:
		menu_activate();
		break;
	case EV_TICK:
		key_timer();
		count_ui_timer();
		break;
	case EV_SMAUL_PUSH:
		menu_button_smaul();
		break;
	}

	if (event != EV_TICK) {
		enable_lcd_backlight();
		reset_ui_timer();
	}

	menu_repaint();
}

void ui_init(void)
{
	initTimers();
}

#endif // __NO_INCLUDE_AVR
