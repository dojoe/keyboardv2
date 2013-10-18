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

#include "Descriptors.h"

#include <LUFA/Drivers/USB/USB.h>

#include "onewire.h"
#include "mc-eeprom.h"
#include "key.h"

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

void handle_command(void)
{
	printf("%s\n", str);

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
}

void SetupHardware(void);

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	static uint8_t key_present_state = 0;
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	stdout = &USBSerialStream;

	GlobalInterruptEnable();

	for (;;)
	{
		while (1) {
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
			}

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

	/* Hardware Initialization */
	USB_Init();

	DDRE = 1 << PE6;
	PORTE = 0;

	ow_init();
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

