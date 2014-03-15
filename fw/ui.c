
#include <stdio.h>
#include "hw.h"
#include "lcd_drv.h"
#include "menu-defs.h"
#include "common.h"
#include "panel.h"
#include "ui.h"
#include "key.h"

uint8_t  ui_state = UIS_IDLE;
uint8_t  selected_key = 0;
uint16_t selected_time = 0;
uint8_t  ui_timer = 0;

extern uint8_t expired_key;

static void print_missing_key(void) {
	uint8_t slot = expired_key - 1;

	if (!expired_key) {
		lcd_printfP(0, PSTR(""));
	} else if (slot < MAX_KEYS) {
		lcd_printfP(0, PSTR("Key %s missing"), keys[slot].eep.key.name);
	} else {
		lcd_printfP(0, PSTR("Pizza %d done"), slot - MAX_KEYS + 1);
	}
}

static void menu_repaint(void) {
	uint8_t n;

	switch (ui_state) {
	case UIS_IDLE:
		print_missing_key();
		keytimer_displayupdate();
		break;

	case UIS_MENU_PIZZA1:
	case UIS_MENU_PIZZA2:
	case UIS_MENU_PIZZA3:
		n = ui_state - UIS_MENU_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			lcd_printfP(0, PSTR("Pizzatimer %d Off"), n + 1);
		} else {
			lcd_printfP(0, PSTR("Pizzatimer %d"), n + 1);
		}
		break;

	case UIS_MENU_FIND_KEY:
		lcd_printfP(0, PSTR("Locate key"));
		break;

	case UIS_MENU_BOOTLOADER:
		lcd_printfP(0, PSTR("Enter bootloader"));
		break;

	case UIS_SELECT_TIME:
		lcd_printfP(1, PSTR("%02i:%02i"), selected_time / 60,
				selected_time % 60);
		break;

	case UIS_FIND_KEY:
		if (keys[selected_key].state == KS_VALID)
			lcd_printfP(1, PSTR("%s"), keys[selected_key].eep.key.name);
		else
			lcd_printfP(1, (keys[selected_key].state == KS_EMPTY) ? PSTR("No key plugged") : PSTR("Read error"));
		keyled_on(selected_key);
		break;
	}
}

/**
 * this method is used internally to reset the timer for ending the menu mode.
 */
static void reset_ui_timer(void) {
	switch (ui_state) {
	case UIS_MENU_PIZZA1:
	case UIS_MENU_PIZZA2:
	case UIS_MENU_PIZZA3:
	case UIS_MENU_FIND_KEY:
	case UIS_MENU_BOOTLOADER:
		ui_timer = MENU_TIMEOUT_SECONDS;
		break;

	case UIS_SELECT_TIME:
	case UIS_FIND_KEY:
		ui_timer = MENU_TIMEOUT_SELECT_SECONDS;
		break;

	default:
		ui_timer = 0;
		break;
	}
}

static void menu_enter(void) {
	ui_state = UIS_MENU_FIND_KEY;
	reset_ui_timer();
	lcd_printfP(1, PSTR(""));
}

static void ui_to_idle(void) {
	ui_state = UIS_IDLE;
	keyleds_off();
}

static void apply_timer(void) {
	setKeyTimeout(selected_key, selected_time);
	ui_to_idle();
}

static void count_ui_timer(void) {
	if (ui_timer && !(--ui_timer)) {
		if (ui_state == UIS_SELECT_TIME) {
			apply_timer();
		} else {
			ui_to_idle();
		}
	}
}

static void menu_activate(void) {
	uint8_t n;

	switch (ui_state) {
	// enable the menu;
	case UIS_IDLE:
		menu_enter();
		break;

	case UIS_MENU_PIZZA1:
	case UIS_MENU_PIZZA2:
	case UIS_MENU_PIZZA3:
		n = ui_state - UIS_MENU_PIZZA1;
		if (pizzatimer_running(n) != 0) {
			pizzatimer_clear(n);
			ui_to_idle();
		} else {
			selected_time = PIZZA_TIMER_DEFAULT_TIME;
			selected_key = KEY_ID_PIZZATIMER_OFFSET + n;
			ui_state = UIS_SELECT_TIME;
		}
		break;

	case UIS_MENU_FIND_KEY:
		ui_state = UIS_FIND_KEY;
		selected_key = 0;
		break;

	case UIS_MENU_BOOTLOADER:
		lcd_printfP(1, PSTR("Obai! o/"));
		call_bootloader();
		break;

	case UIS_SELECT_TIME:
		apply_timer();
		break;

	case UIS_FIND_KEY:
		ui_to_idle();
		break;
	}
}

static void menu_button_forward(void) {
	switch (ui_state) {
	case UIS_MENU_FIND_KEY:
	case UIS_MENU_PIZZA1:
	case UIS_MENU_PIZZA2:
	case UIS_MENU_PIZZA3:
		ui_state++;
		break;

	case UIS_MENU_BOOTLOADER:
		ui_state = UIS_MENU_FIND_KEY;
		break;

	case UIS_SELECT_TIME:
		selected_time = max(60, selected_time+60);
		break;

	case UIS_FIND_KEY:
		if (selected_key == MAX_KEYS - 1)
			selected_key = 0;
		else
			selected_key++;
		break;
	}
}

static void menu_button_back(void) {
	switch (ui_state) {
	case UIS_MENU_FIND_KEY:
		ui_state = UIS_MENU_BOOTLOADER;
		break;

	case UIS_MENU_PIZZA1:
	case UIS_MENU_PIZZA2:
	case UIS_MENU_PIZZA3:
	case UIS_MENU_BOOTLOADER:
		ui_state--;
		break;

	case UIS_SELECT_TIME:
		selected_time = min(99*60, selected_time-60);
		break;

	case UIS_FIND_KEY:
		if (selected_key == 0)
			selected_key = MAX_KEYS - 1;
		else
			selected_key--;
		break;
	}
}

static void menu_button_smaul(void) {
	switch (ui_state) {
	case UIS_IDLE:
		key_smaul();
		break;
	default:
		ui_to_idle();
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
	case EV_ENCODER_CW:
		menu_button_forward();
		break;
	case EV_ENCODER_CCW:
		menu_button_back();
		break;
	case EV_ENCODER_PUSH:
		menu_activate();
		break;
	case EV_SMAUL_PUSH:
		menu_button_smaul();
		break;
	case EV_TICK:
		key_timer();
		count_ui_timer();
		break;
	}

	if (event != EV_TICK && event < EV_KEY_CHANGE0) {
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
