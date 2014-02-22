#include <string.h>
#include <avr/io.h>
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
	KMS_READ     = 4,
	KMS_READ_OK  = 5,
	KMS_READ_ERR = 6,
	KMS_DISABLE  = 7,
};

uint8_t current_key = 0;
uint8_t keymgr_state = KMS_IDLE;
uint8_t wait_ms;

struct key_eeprom_data key_read_data;

struct key_socket keys[MAX_KEYS];

void key_init(void)
{
	memset(&keys, 0, sizeof(keys));
}

static void key_read_cb(uint8_t success)
{
	ow_disconnect();
	keymgr_state = success ? KMS_READ_OK : KMS_READ_ERR;
}

static void key_disable_and_next(void)
{
	PWR_PORT &= ~PWR_BIT;
	PWR_DDR  |=  PWR_BIT;
	wait_ms = global_ms_timer;
	keymgr_state = KMS_DISABLE;
}

static void set_key_state(uint8_t state)
{
	if (keys[current_key].state != state) {
		push_event(EV_KEY_CHANGE0 + current_key);
		keys[current_key].state = state;
	}
}

static uint8_t key_validate(void)
{
	uint16_t crc = 0xFFFF;
	uint8_t i;
	uint8_t *data = (uint8_t *)&key_read_data;

	for (i = 0; i < sizeof(key_read_data) - sizeof(crc); i++)
		crc = _crc16_update(crc, data[i]);

	return crc == key_read_data.crc16;
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
		shiftregs.key_sel = current_key;
		shiftregs.key_en  = 0; // note negative logic
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

		eep_read(0, sizeof(key_read_data), &key_read_data, key_read_cb);
		keymgr_state = KMS_READ;
		break;

	case KMS_READ:
		/* State will be changed by callback */
		break;

	case KMS_READ_ERR:
		set_key_state(KS_READ_ERROR);
		key_disable_and_next();
		break;

	case KMS_READ_OK:
		if (keys[current_key].state != KS_VALID || memcmp(&key_read_data, &keys[current_key].eep, sizeof(key_read_data))) {
			if (!key_validate()) {
				set_key_state(KS_CRC_ERROR);
			} else {
				memcpy(&keys[current_key].eep, &key_read_data, sizeof(key_read_data));
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
