#include <avr/wdt.h>
#include <LUFA/Drivers/USB/USB.h>
#include "common.h"

uint8_t event_queue[EVENT_QUEUE_SIZE];
volatile uint8_t event_queue_head = 0, event_queue_tail = 0;

static inline uint8_t event_queue_full(void)
{
	return ((event_queue_head - event_queue_tail) == EVENT_QUEUE_SIZE);
}

static inline uint8_t event_queue_empty(void)
{
	return (event_queue_head == event_queue_tail);
}

void push_event(uint8_t event)
{
	// This might be called from the timer interrupt, so disable interrupts to ensure atomicity
	uint8_t saveflags = SREG;
	cli();

	if (!event_queue_full()) // drop event if queue full
		event_queue[(event_queue_head++) & (EVENT_QUEUE_SIZE - 1)] = event;

	SREG = saveflags;
}

uint8_t get_event(void)
{
	return event_queue_empty() ? EV_NONE : event_queue[(event_queue_tail++) & (EVENT_QUEUE_SIZE - 1)];
}

/* Bootloader jump code adapted from http://www.fourwalledcubicle.com/files/LUFA/Doc/120219/html/_page__software_bootloader_start.html */

uint32_t boot_key ATTR_NO_INIT;
uint8_t g_test_mode ATTR_NO_INIT;

#define BOOT_KEY_MAGIC  0xCAFEBABE
#define TEST_MODE_MAGIC 0xABADF00D

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
	g_test_mode = ((MCUSR & (1 << WDRF)) && (boot_key == TEST_MODE_MAGIC));
	boot_key = 0;
}

void watchdog_reset(uint8_t where)
{
	USB_Disable();
	cli();

	if (where == WDR_BOOTLOADER)
		boot_key = BOOT_KEY_MAGIC;
	else if (where == WDR_TESTMODE)
		boot_key = TEST_MODE_MAGIC;

	_delay_ms(2000);
	wdt_enable(WDTO_30MS);
	while (1);
}
