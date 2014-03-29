#include <stdio.h>
#include <string.h>
#include "hw.h"
#include "lcd_drv.h"
#include "common.h"
#include "key_timer.h"
#include "key.h"
#include "config.h"

// the key-timers.
int16_t keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];
uint8_t expired_key = 0;

void initTimers(void)
{
	uint8_t i;

	for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
		keyTimers[i] = -1;
	}
}

static void key_timer_expired(uint8_t slot)
{
	if (slot >= MAX_KEYS) {
		beeper_start(slot - MAX_KEYS + BEEP_PIZZA1);
	} else {
		beeper_start(BEEP_KEYMISSING);
	}

	smaul_pulse(250);
	expired_key = slot + 1;
}

static void find_next_expired_timer(uint8_t beep_code_if_none)
{
	uint8_t i;

	// check if there's another key that needs handling.
	for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
		if (keyTimers[i] == 0) {
			key_timer_expired(i);
			return;
		}
	}

	expired_key = 0;
	beeper_start(beep_code_if_none);
	smaul_off();
}

void key_smaul(void)
{
	uint8_t i;

	// is there a key missing?
	for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
		if (keyTimers[i] == 0) {
			// clear the first key timer -> set + 5 Minutes.
			keyTimers[i] = 300;
			break;
		}
	}

	find_next_expired_timer(BEEP_OFF);
}

void key_timer(void)
{
	uint8_t i;

	for (i = 0; i < MAX_KEYS + NUM_PIZZA_TIMERS; i++) {
		if (keyTimers[i] > 0) {
			keyTimers[i]--;
			if (keyTimers[i] == 0) {
				key_timer_expired(i);
			}
		}
	}
}

void setKeyTimeout(uint8_t key, int16_t time)
{
	if (key >= KEY_ID_PIZZATIMER_OFFSET) {
		keyTimers[MAX_KEYS + (key - KEY_ID_PIZZATIMER_OFFSET)] = time;
		return;
	}

	keyTimers[key] = time;
}

void annoyPeopleForInvalidKey(void)
{

}

void key_change(void)
{
	uint8_t i, j;

	// current keyboard state: keys[]
	// current keyboard configuration: config.keys[]

	// CASE 1: A key defined in the config is not in the keyboard
	for (i = 0; i < MAX_KEYS; i++) {
		// if the key is not plugged in, this keyboard?
		if (config.keys[i].id) {
			uint8_t keyIsPresent = 0;

			// this key should be available. check if it is.
			for (j = 0; j < MAX_KEYS; j++) {
				if (keys[j].state == KS_VALID
						&& keys[j].eep.key.id == config.keys[i].id
						&& keys[j].eep.kb.id == config.kb.id) {
					// key is present.
					keyIsPresent = 1;
					break;
				}
			}

			// if the key is not present, check if we need an alarm.
			if (keyIsPresent == 0 && keyTimers[i] == -1) {
				// set a timer because the key vanished
				setKeyTimeout(i, config.keys[i].dfl_timeout);
				if (!expired_key)
					beeper_start(BEEP_SINGLE);
			} else if (keyIsPresent == 1 && keyTimers[i] >= 0) {
				// unset a timer because the key came back.
				setKeyTimeout(i, -1);
				find_next_expired_timer(BEEP_SINGLE);
			}
		}
	}

	// CASE 2: A key is present but it does not belong to the current keyboard.
	for (i = 0; i < MAX_KEYS; i++) {
		// if the key is plugged, check if it's in our config.
		if (keys[i].state == KS_VALID) {
			int keyBelongsToKeyboard = 0;

			for (j = 0; j < MAX_KEYS; j++) {
				if (keys[i].eep.key.id == config.keys[j].id) {
					// this key is expected from our config.
					keyBelongsToKeyboard = 1;
					break;
				}
			}

			// check if the key belongs to this keyboard... if not - warn
			if (keyBelongsToKeyboard == 0) {
				annoyPeopleForInvalidKey();
			}
		} else if (keys[i].state != KS_EMPTY) {
			// there was some kind of error - we should do something about this... like annoy anyone close to the keyboard.
			annoyPeopleForInvalidKey();
		}
	}
}

static void print_time(int16_t timeInSeconds)
{
	if (timeInSeconds == -1) {
		lcd_print_update_P(1, PSTR("--- "));
	} else if (timeInSeconds < 60) {
		// print <timeInSeconds>s
		lcd_print_update_P(1, PSTR("%2ds "), timeInSeconds);
	} else {
		// print <timeInSeconds / 60>m
		lcd_print_update_P(1, PSTR("%2dm "), timeInSeconds / 60);
	}
}

/** 
 * helper to check if a pizza timer is running
 * @param n pizza timer number, must be between 0 and MAX_KEYS
 */
uint8_t pizzatimer_running(uint8_t n)
{
	if (n < 0 || n >= NUM_PIZZA_TIMERS) {
		return 0;
	}

	return keyTimers[MAX_KEYS + n] != -1;
}

/** 
 * helper to clear a pizza timer.
 */
void pizzatimer_clear(uint8_t n)
{
	if (n >= 0 && n < NUM_PIZZA_TIMERS) {
		keyTimers[MAX_KEYS + n] = -1;
	}
}

/**  
 * helper to check what time the minimum key has, does not include pizza timers.
 */
static int16_t getMinimumKeyTimer(void)
{
	uint8_t i;
	int16_t min = INT16_MAX;

	for (i = 0; i < MAX_KEYS; i++)
		if (keyTimers[i] >= 0 && keyTimers[i] < min)
			min = keyTimers[i];

	return (min == INT16_MAX) ? -1 : min;
}

void keytimer_display_update(void)
{
	lcd_print_start(1);
	print_time(keyTimers[MAX_KEYS + 0]);
	print_time(keyTimers[MAX_KEYS + 1]);
	print_time(keyTimers[MAX_KEYS + 2]);
	print_time(getMinimumKeyTimer());
	lcd_print_end(1);
}

