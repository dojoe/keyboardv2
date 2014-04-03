#include <stdio.h>
#include <string.h>
#include "hw.h"
#include "lcd_drv.h"
#include "common.h"
#include "key_timer.h"
#include "key.h"
#include "config.h"
#include "ui.h"

// the key-timers.
int16_t keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];
uint8_t keyMissing[MAX_KEYS] = { 0 };

/* Number of keys missing that need the rotating light */
static uint8_t rotlight_counter = 0;

void initTimers(void)
{
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(keyTimers); i++) {
		keyTimers[i] = -1;
	}
}

static uint8_t check_expired_timers(void)
{
	uint8_t i;

	// check if there's another key that needs handling.
	for (i = 0; i < ARRAY_SIZE(keyTimers); i++) {
		if (keyTimers[i] == 0) {
			ui_set_timer_expired(i);
			return 0;
		}
	}

	ui_clear_timer_expired();
	return 1;
}

uint8_t clearKeyTimeout(uint8_t key)
{
	keyTimers[key] = -1;
	return check_expired_timers();
}

void key_smaul(void)
{
	uint8_t i;

	// is there a key missing?
	for (i = 0; i < ARRAY_SIZE(keyTimers); i++) {
		if (keyTimers[i] == 0) {
			// clear the first key timer -> set + 5 Minutes.
			keyTimers[i] = 300;
			check_expired_timers();
			break;
		}
	}
}

void key_timer(void)
{
	uint8_t i, expired = 0;

	for (i = 0; i < ARRAY_SIZE(keyTimers); i++)
		if (keyTimers[i] > 0 && --keyTimers[i] == 0)
			expired = 1;

	if (expired)
		check_expired_timers();
}

static uint8_t check_key_errors(void)
{
	uint8_t config_idx, slot_idx;

	for (slot_idx = 0; slot_idx < MAX_KEYS; slot_idx++) {
		struct key_socket *k = keys + slot_idx;

		// if the key is plugged, check if it's in our config.
		if (k->state == KS_VALID) {
			int keyBelongsToKeyboard = 0;

			// If key belongs to a different keyboard, warn!
			if (k->eep.kb.id != config.kb.id) {
				ui_set_key_error(UIF_KEY_ERROR_OTHER_KB, slot_idx);
				return 1;
			}

			for (config_idx = 0; config_idx < MAX_KEYS; config_idx++) {
				if (k->eep.key.id == config.keys[config_idx].id) {
					// this key is expected from our config.
					keyBelongsToKeyboard = 1;
					break;
				}
			}

			// If key claims to belong to this keyboard, but is not known in the config -- warn!
			if (keyBelongsToKeyboard == 0) {
				ui_set_key_error(UIF_KEY_ERROR_UNKNOWN, slot_idx);
				return 1;
			}
		} else if (k->state != KS_EMPTY) {
			// there was some kind of error - we should do something about this... like annoy anyone close to the keyboard.
			ui_set_key_error(UIF_KEY_ERROR_READ_ERR, slot_idx);
			return 1;
		}
	}

	ui_clear_key_error();
	return 0;
}

static void setKeyMissing(uint8_t config_idx)
{
	struct key_info *k = config.keys + config_idx;

	keyMissing[config_idx] = 1;
	if (k->flags & KF_BEEP)
		setKeyTimeout(config_idx, k->dfl_timeout);
	if ((k->flags & KF_ROTLIGHT) && rotlight_counter++ == 0)
		rotlight_on();

	if (!ui_flags)
		beeper_start(BEEP_SINGLE);
	lcd_printfP(0, PSTR("Obai, %s key o/"), k->name);

	if (k->flags & KF_BEEP)
		ui_select_time(config_idx, k->dfl_timeout, k->max_timeout);
	else
		ui_short_message();
}

static void setKeyReturned(uint8_t config_idx)
{
	struct key_info *k = config.keys + config_idx;

	keyMissing[config_idx] = 0;
	if ((k->flags & KF_ROTLIGHT) && --rotlight_counter == 0)
		rotlight_off();

	if (clearKeyTimeout(config_idx)) {
		beeper_start(BEEP_SINGLE);
		lcd_printfP(0, PSTR("Ohai, %s key =D"), config.keys[config_idx].name);
		ui_short_message();
	}
}

static void check_missing_keys(void)
{
	uint8_t config_idx, slot_idx;

	for (config_idx = 0; config_idx < MAX_KEYS; config_idx++) {
		// if the key is not plugged in, this keyboard?
		if (config.keys[config_idx].id) {
			uint8_t keyIsPresent = 0;

			// this key should be available. check if it is.
			for (slot_idx = 0; slot_idx < MAX_KEYS; slot_idx++) {
				if (keys[slot_idx].state == KS_VALID
						&& keys[slot_idx].eep.key.id == config.keys[config_idx].id
						&& keys[slot_idx].eep.kb.id == config.kb.id) {
					// key is present.
					keyIsPresent = 1;
					break;
				}
			}

			// if the key is not present, check if we need an alarm.
			if (keyIsPresent == 0 && !keyMissing[config_idx])
				setKeyMissing(config_idx);
			else if (keyIsPresent == 1 && keyMissing[config_idx])
				setKeyReturned(config_idx);
		}
	}
}

void key_change(void)
{
	// current keyboard state: keys[]
	// current keyboard configuration: config.keys[]

	uint8_t slot_idx;

	// Check that we passed the initial phase where we're not sure about the state of our keys.
	for (slot_idx = 0; slot_idx < MAX_KEYS; slot_idx++)
		if (keys[slot_idx].state == KS_UNKNOWN)
			return;

	if (check_key_errors())
		return;

	check_missing_keys();
}
