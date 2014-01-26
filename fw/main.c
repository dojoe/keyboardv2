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

static void SetupHardware(void)
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

	shiftreg_reset();

	// Set up T/C 1 for 8-bit fast PWM running at F_CPU/256 (64kHz), resulting in a PWM period of 250 Hz
	// Also, use inverted PWM so it's possible to turn the pin off completely
	set_lcd_led(LCD_LED_DIM);
	set_smaul_led(0);
	TCCR1A = (1 << WGM10) | (3 << COM1A0) | (3 << COM1B0) | (0 << COM1C0);
	TCCR1B = (1 << WGM12) | (4 << CS10);

	ow_init();
	key_init();
	load_config();

	lcd_init();
	lcd_xy(0, 0);
	lcd_blank(32);

	/* Set up T/C3 to run at CLK/64 and do 8-bit PWM, leading to an OCR int at F_CPU / 16k, i.e. roughly 1kHz */
	TCNT3 = 0;
	TIMSK3 = 1 << TOIE3;
	TIFR3  = 1 << TOV3;
	TCCR3A = 1 << WGM30;
	TCCR3B = 1 << WGM32 | 3 << CS30;

	usb_init();
}

int main(void) __attribute__((OS_main));
int main(void)
{
	uint8_t rot_value = 0;
	char str[100];
	SetupHardware();

	sei();

	for (;;)
	{
		usb_poll();
		eep_poll();
		key_poll();

		switch (get_event()) {
		case EV_ENCODER_CW:
			rot_value++;
			break;
		case EV_ENCODER_CCW:
			rot_value--;
			break;
		case EV_ENCODER_PUSH:
			rot_value = 0;
			break;
		case EV_SMAUL_PUSH:
			if (rot_value == 254)
				call_bootloader();
			else if (rot_value >= 70)
				keyled_blink(rot_value - 70);
			else if (rot_value == 69)
				keyleds_off();
			else if (rot_value >= 60)
				keyled_on(rot_value - 60);
			else if (rot_value > 40)
				smaul_blink(rot_value - 40);
			else if (rot_value > 20)
				smaul_pulse(rot_value - 20);
			else if (rot_value == 20)
				smaul_off();
			else if (rot_value > 10)
				set_lcd_backlight(rot_value == 11);
      else if (rot_value == 250) 
        call_menu();
			else
				beeper_start(rot_value);
			break;
		case EV_TICK:
			//beeper_start(BEEP_SINGLE);
			break;
		}
		//if (ow_done()) {
		sprintf(str, "%03d", rot_value);
		lcd_xy(0, 0);
		lcd_puts(str);
		//}
	}
}
