#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "onewire.h"
#include "mc-eeprom.h"

enum eep_state_e {
	IDLE,
	READ,
	WR_ENABLE,
	WR_WRITE,
	WR_WAIT,
	PR_ENABLE,
	ER_ENABLE,
	ERPR_WRITE,
	ERPR_WAIT,
};

static volatile enum eep_state_e eep_state;
static uint16_t eep_addr;
static uint16_t eep_remaining;
static uint8_t *eep_buf;
static eep_callback eep_cb;

enum eep_cmd_e {
	CMD_READ  = 0x03,
	CMD_CRRD  = 0x06,
	CMD_WRITE = 0x6C,
	CMD_WREN  = 0x96,
	CMD_WRDI  = 0x91,
	CMD_RDSR  = 0x05,
	CMD_WRSR  = 0x6E,
	CMD_ERAL  = 0x6D,
	CMD_SETAL = 0x67,
};

enum eep_status_e {
	SR_WIP = 1,
	SR_WEL = 2,
	SR_BP0 = 4,
	ST_BP1 = 8,
};

#define EEP_READ_CHUNK 64
#define EEP_WRITE_PAGE_SIZE 16

static void eep_do_read(void)
{
	uint8_t size = (eep_remaining > EEP_READ_CHUNK) ? EEP_READ_CHUNK : eep_remaining;

	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = CMD_READ;
	ow_buffer[3] = eep_addr >> 8;
	ow_buffer[4] = eep_addr & 0xFF;
	ow_read_buf(5, size, eep_buf);

	eep_remaining -= size;
	eep_addr += size;
	eep_buf += size;
}

static void eep_enable_write(void)
{
	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = CMD_WREN;
	ow_write(3);
}

static void eep_read_status(void)
{
	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = CMD_RDSR;
	ow_read(3, 1);
}

static void eep_do_write(void)
{
	uint8_t page_left = EEP_WRITE_PAGE_SIZE - (eep_addr & (EEP_WRITE_PAGE_SIZE - 1));
	uint8_t size = (eep_remaining > page_left) ? page_left : eep_remaining;

	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = CMD_WRITE;
	ow_buffer[3] = eep_addr >> 8;
	ow_buffer[4] = eep_addr & 0xFF;
	memcpy(ow_buffer + 5, eep_buf, size);
	ow_write(5 + size);

	eep_remaining -= size;
	eep_addr += size;
	eep_buf += size;
}

static void eep_set_protect(void)
{
	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = CMD_WRSR;
	ow_buffer[3] = eep_addr;
	ow_write(4);
}

static void eep_do_erase(void)
{
	ow_buffer[0] = 0x55;
	ow_buffer[1] = 0xA0;
	ow_buffer[2] = eep_addr;
	ow_write(3);
}

void eep_read(uint16_t addr, uint16_t size, void *buf, eep_callback cb)
{
	eep_addr = addr;
	eep_remaining = size;
	eep_buf = buf;
	eep_cb = cb;
	eep_state = READ;
	eep_do_read();
}

void eep_write(uint16_t addr, uint16_t size, void *buf, eep_callback cb)
{
	eep_addr = addr;
	eep_remaining = size;
	eep_buf = buf;
	eep_cb = cb;
	eep_state = WR_ENABLE;
	eep_enable_write();
}

void eep_protect(uint8_t protbits, eep_callback cb)
{
	eep_state = PR_ENABLE;
	eep_addr = protbits;
	eep_cb = cb;
	eep_enable_write();
}

void eep_erase(uint8_t erase_value, eep_callback cb)
{
	eep_state = ER_ENABLE;
	eep_addr = erase_value;
	eep_cb = cb;
	eep_enable_write();
}

void eep_poll(void)
{
	enum eep_state_e eep_state_copy = eep_state;

	if (eep_state_copy == IDLE || !ow_done())
		return;

	if (ow_error()) {
		eep_state = IDLE;
		eep_cb(0);
		return;
	}

	switch (eep_state_copy) {
	case READ:
		if (eep_remaining) {
			eep_do_read();
		} else {
			eep_state = IDLE;
			eep_cb(1);
		}
		break;
	case WR_ENABLE:
		eep_do_write();
		eep_state = WR_WRITE;
		break;
	case WR_WRITE:
		eep_read_status();
		eep_state = WR_WAIT;
		break;
	case WR_WAIT:
		if (ow_buffer[3] & SR_WIP) {
			eep_read_status();
		} else if (eep_remaining) {
			eep_enable_write();
			eep_state = WR_ENABLE;
		} else {
			eep_state = IDLE;
			eep_cb(1);
		}
		break;
	case PR_ENABLE:
		eep_set_protect();
		eep_state = ERPR_WRITE;
		break;
	case ER_ENABLE:
		eep_do_erase();
		eep_state = ERPR_WRITE;
		break;
	case ERPR_WRITE:
		eep_read_status();
		eep_state = ERPR_WAIT;
		break;
	case ERPR_WAIT:
		if (ow_buffer[3] & SR_WIP) {
			eep_read_status();
		} else {
			eep_state = IDLE;
			eep_cb(1);
		}
		break;
	default:
		break;
	}
}
