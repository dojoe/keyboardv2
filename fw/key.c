#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/crc16.h>
#include "common.h"
#include "panel.h"
#include "key.h"
#include "onewire.h"
#include "mc-eeprom.h"

enum keymgr_state {
	KMS_IDLE     = 0,
	KMS_SELECT   = 1,
	KMS_DETECT   = 2,
	KMS_ENABLE   = 3,
	KMS_XFER     = 4,
	KMS_XFER_OK  = 5,
	KMS_XFER_ERR = 6,
	KMS_DISABLE  = 7,
};

static uint8_t current_key = 0;
static uint8_t keymgr_state = KMS_IDLE;
static uint8_t wait_ms;

static uint8_t programming = 0;
static key_program_cb program_cb;

struct key_eeprom_data key_xfer_data;

struct key_socket keys[MAX_KEYS];

void key_init(void)
{
	memset(&keys, 0, sizeof(keys));
}

static void key_xfer_cb(uint8_t success)
{
	ow_disconnect();
	keymgr_state = success ? KMS_XFER_OK : KMS_XFER_ERR;
}

static void key_disable(void)
{
	PWR_PORT &= ~PWR_BIT;
	PWR_DDR  |=  PWR_BIT;
}

static void key_disable_and_next(void)
{
	key_disable();
	programming = 0;
	wait_ms = global_ms_timer;
	keymgr_state = KMS_DISABLE;
}

static void set_key_state(uint8_t state)
{
	struct key_socket *k = keys + current_key;

	if (k->state != state) {
		if (k->new_state != state) {
			k->new_state = state;
			k->new_state_debounce = 2;
		} else {
			if (!(--k->new_state_debounce)) {
				push_event(EV_KEY_CHANGE);
				k->state = state;
			}
		}
	}
}

static uint16_t calc_key_crc(void)
{
	uint16_t crc = 0xFFFF;
	uint8_t i;

	uint8_t *data = (uint8_t *)&key_xfer_data;

	for (i = 0; i < sizeof(key_xfer_data) - sizeof(crc); i++)
		crc = _crc16_update(crc, data[i]);

	return crc;
}

static uint8_t key_validate(void)
{
	return calc_key_crc() == key_xfer_data.crc16;
}

void key_program(uint8_t slot, struct key_eeprom_data *data, key_program_cb cb)
{
	/* Abort whatever the key manager was just in the process of doing */
	eep_abort();
	key_disable();
	keymgr_state = KMS_IDLE;
	current_key = slot;
	program_cb = cb;
	programming = 1;
	key_xfer_data = *data;
	key_xfer_data.crc16 = calc_key_crc();
}

#include <stdio.h>
#include "lcd_drv.h"

static inline uint8_t wait_done(uint8_t ms)
{
	uint8_t x = global_ms_timer - wait_ms;
	return (x < ms);
}

void key_poll(void)
{
	if (0) {
		char str[30];
		sprintf(str, " %d %d %03d %03d %d %d %d %d %d %d %d %d", keymgr_state, current_key, global_ms_timer, wait_ms,
				keys[0].state, keys[1].state, keys[2].state, keys[3].state, keys[4].state, keys[5].state, keys[6].state, keys[7].state);
		lcd_puts(str);
	}

	switch (keymgr_state) {
	case KMS_IDLE:
		cli();
		shiftregs.key_sel = current_key;
		shiftregs.key_en  = 0; // note negative logic
		sei();
		shiftreg_update();
		keymgr_state = KMS_SELECT;
		break;

	case KMS_SELECT:
		if (!shiftreg_done())
			break;

		/* Detect if plug fully inserted.
		 *
		 * If the plug is fully inserted, the tip switch is open, so Tip is no longer shorted to GND.
		 * Check by switching Tip to pull-up and reading whether it stays high.
		 * Incidentally, if for some reason Tip is shorted to Sleeve, it will look just like a closed switch.
		 * As an additional bonus, the onewire code pulls Ring to GND when disconnected
		 * so we can detect shorts between Ring and Tip, too.
		 */
		PWR_DDR   &= ~PWR_BIT;
		PWR_PORT  |=  PWR_BIT;

		keymgr_state = KMS_DETECT;
		wait_ms = global_ms_timer;
		break;

	case KMS_DETECT:
		if (wait_done(2))
			break;

		if (!(PWR_PIN & PWR_BIT)) {
			if (programming)
				program_cb(KS_EMPTY);
			else
				set_key_state(KS_EMPTY);

			key_disable_and_next();
			break;
		}

		/* Turn on the key power */
		PWR_PORT |= PWR_BIT;
		PWR_DDR  |= PWR_BIT;
		wait_ms = global_ms_timer;
		keymgr_state = KMS_ENABLE;
		break;

	case KMS_ENABLE:
		if (wait_done(2))
			break;

		if (programming)
			eep_write(0, sizeof(key_xfer_data), &key_xfer_data, key_xfer_cb);
		else
			eep_read(0, sizeof(key_xfer_data), &key_xfer_data, key_xfer_cb);
		keymgr_state = KMS_XFER;
		break;

	case KMS_XFER:
		/* State will be changed by callback */
		break;

	case KMS_XFER_ERR:
		if (programming)
			program_cb(KS_READ_ERROR);
		else
			set_key_state(KS_READ_ERROR);

		key_disable_and_next();
		break;

	case KMS_XFER_OK:
		if (programming) {
			program_cb(KS_VALID);
			key_disable_and_next();
			break;
		}

		if (keys[current_key].state != KS_VALID || memcmp(&key_xfer_data, &keys[current_key].eep, sizeof(key_xfer_data))) {
			if (!key_validate()) {
				set_key_state(KS_CRC_ERROR);
			} else {
				memcpy(&keys[current_key].eep, &key_xfer_data, sizeof(key_xfer_data));
				set_key_state(KS_VALID);
			}
		}

		key_disable_and_next();
		break;

	case KMS_DISABLE:
		if (wait_done(2))
			break;

		current_key = (current_key >= MAX_KEYS - 1) ? 0 : (current_key + 1);
		keymgr_state = KMS_IDLE;
		break;
	}
}
