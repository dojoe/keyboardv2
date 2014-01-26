/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Descriptors.h"

#include <LUFA/Drivers/USB/USB.h>

#include "onewire.h"
#include "mc-eeprom.h"
#include "key.h"
#include "lcd_drv.h"
#include "panel.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 0,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;

static char str[100];
static uint8_t strptr = 0;

void eep_normal_cb(int success)
{
	printf("%s\n", success ? "yay" : "nay");
	ow_disconnect();
	key_off();
}

void eep_read_cb(int success)
{
	if (success) {
		int i;
		for (i = 0; i < sizeof(str); i++)
			printf("%02x ", str[i]);
		str[sizeof(str) - 1] = 0;
		printf("\n%s\n", str);
	}
	eep_normal_cb(success);
}

void call_bootloader(void) __attribute__((noreturn));

void handle_command(void)
{
	printf("%s\n", str);

	switch (str[0]) {
	case 'd':
		set_lcd_led(atoi(str+1));
		break;
	case 's':
		set_smaul_led(atoi(str+1));
		break;
	case 'l':
		shiftregs.leds = atoi(str + 1);
		shiftreg_update();
		break;
	case 'm':
		shiftregs.key_sel = atoi(str + 1);
		shiftregs.key_en  = (str[1] == 'n');
		shiftreg_update();
		break;
	case 'b':
		call_bootloader();
		break;
	case 'r':
		key_on();
		eep_read(0, sizeof(str), str, eep_read_cb);
		break;
	}

#if 0
	if (!key_present()) {
		printf("nope\n");
		return;
	}

	key_on();

	switch (str[0]) {
	case 'E':
		eep_erase(EEP_ERASE_FF, eep_normal_cb);
		break;
	case 'e':
		eep_erase(EEP_ERASE_00, eep_normal_cb);
		break;
	case 'P':
		eep_protect(EEP_PROT_ALL, eep_normal_cb);
		break;
	case 'p':
		eep_protect(EEP_PROT_NONE, eep_normal_cb);
		break;
	case 'r':
		eep_read(0, sizeof(str), str, eep_read_cb);
		break;
	case 'w':
		eep_write(0, strlen(str) - 1, str + 2, eep_normal_cb);
		break;
	}
#endif
}

void SetupHardware(void);

/* Bootloader jump code adapted from http://www.fourwalledcubicle.com/files/LUFA/Doc/120219/html/_page__software_bootloader_start.html */

uint32_t boot_key ATTR_NO_INIT;

#define BOOT_KEY_MAGIC 0xCAFEBABE

void bootloader_check(void) ATTR_INIT_SECTION(3);
void bootloader_check(void)
{
	#define FLASH_SIZE_BYTES 32768
	#define BOOTLOADER_SEC_SIZE_BYTES 4096
	#define BOOTLOADER_START_ADDRESS  (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)

	if (((MCUSR & (1 << PORF)) && !(PINE & (1 << PE2))) || ((MCUSR & (1 << WDRF)) && (boot_key == BOOT_KEY_MAGIC))) {
		boot_key = 0;
		((void (*)(void))BOOTLOADER_START_ADDRESS)();
	}
}

void call_bootloader(void)
{
	USB_Disable();
	cli();
	boot_key = BOOT_KEY_MAGIC;
	_delay_ms(2000);
	wdt_enable(WDTO_30MS);
	while (1);
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void) __attribute__((OS_main));
int main(void)
{
	static uint8_t key_present_state = 0;
	uint8_t rot_value = 0;
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	stdout = &USBSerialStream;

	GlobalInterruptEnable();

	for (;;)
	{
		while (1) {
			/*
			if (ow_done()) {
				if (key_present()) {
					if (!key_present_state) {
						key_on();
						eep_read(0, sizeof(str), str, eep_read_cb);
						key_present_state = 1;
					}
				} else {
					key_present_state = 0;
				}
			}*/

			int16_t rc = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			if (rc < 0)
				break;

			if (rc == '\n') {
				str[strptr] = 0;
				handle_command();
				strptr = 0;
			} else if (strptr < sizeof(str) - 1) {
				str[strptr++] = rc;
			}
		}

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		eep_poll();
		switch (next_input_event()) {
		case IN_ENCODER_CW:
			rot_value++;
			break;
		case IN_ENCODER_CCW:
			rot_value--;
			break;
		case IN_ENCODER_PUSH:
			rot_value = 0;
			break;
		case IN_SMAUL_PUSH:
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
			else
				beeper_start(rot_value);
			break;
		}
		if (ow_done()) {
		sprintf(str, "%d   ", rot_value);
		lcd_xy(0, 0);
		lcd_puts(str);
		}
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
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

	lcd_init();
	lcd_xy(0, 0);
	lcd_blank(32);

	/* Set up T/C3 to run at CLK/64 and do 8-bit PWM, leading to an OCR int at F_CPU / 16k, i.e. roughly 1kHz */
	TCNT3 = 0;
	TIMSK3 = 1 << TOIE3;
	TIFR3  = 1 << TOV3;
	TCCR3A = 1 << WGM30;
	TCCR3B = 1 << WGM32 | 3 << CS30;

	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	PORTE |= 1 << PE6;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	PORTE &= ~(1 << PE6);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}
