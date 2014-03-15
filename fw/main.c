#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "onewire.h"
#include "mc-eeprom.h"
#include "key.h"
#include "lcd_drv.h"
#include "panel.h"
#include "usb.h"
#include "config.h"
#include "ui.h"

static void setup(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	//      SS         | SCK        | SOUT       | ENC_A      | ENC_PUSH   | SMAUL_LED  | LCD_LED    | ENC_B
	DDRB  = (1 << PB0) | (1 << PB1) | (1 << PB2) | (0 << PB3) | (0 << PB4) | (1 << PB5) | (1 << PB6) | (0 << PB7);
	PORTB = (0 << PB0) | (0 << PB1) | (0 << PB2) | (1 << PB3) | (1 << PB4) | (0 << PB5) | (0 << PB6) | (1 << PB7);

	//      LCD_E      | LCD_RS
	DDRC  = (1 << PC6) | (1 << PC7);
	PORTC = (0 << PC6) | (0 << PC7);

	//      KEY_PWR    | KEY_SCIO   | M_RX       | M_TX       | LCD_D4     | LCD_D5     | LCD_D6     | LCD_D7
	DDRD  = (0 << PD0) | (0 << PD1) | (0 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
	PORTD = (0 << PD0) | (0 << PD1) | (0 << PD2) | (0 << PD3) | (0 << PD4) | (0 << PD5) | (0 << PD6) | (0 << PD7);

	//      SMAUL_BTN  | TinyMega LED
	DDRE  = (0 << PE2) | (1 << PE6);
	PORTE = (0 << PE2) | (0 << PE6);

	//      LATCH      | LCD_RW     | PF4..PF7 are JTAG
	DDRF  = (1 << PF0) | (1 << PF1);
	PORTF = (0 << PF0) | (0 << PF1);

	ow_init();
	key_init();
	load_config();
	lcd_init();
	panel_init();
	ui_init();

	usb_init();
}

int main(void) __attribute__((OS_main));
int main(void)
{
	setup();

	sei();

	for (;;)
	{
		usb_poll();
		eep_poll();
		key_poll();
		lcd_poll();
		ui_poll();
	}
}
