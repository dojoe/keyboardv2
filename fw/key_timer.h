/** 
 * key_timer.h
 * definitions for the timer stuff.
 */

#ifndef KEY_TIMER_H_
#define KEY_TIMER_H_

extern int16_t keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];
extern uint8_t keyMissing[MAX_KEYS];

void initTimers(void);
void key_change(void);
void key_smaul(void);
void key_timer(void);

static inline uint8_t isKeyMissing(uint8_t key)
{
	return keyMissing[key];
}

static inline void setKeyTimeout(uint8_t key, uint8_t minutes)
{
	keyTimers[key] = minutes * 60;
}

uint8_t clearKeyTimeout(uint8_t key);

static inline uint8_t isKeyTimerRunning(uint8_t key)
{
	return (keyTimers[key] >= 0);
}

/**
 * helper to check if a pizza timer is running
 * @param n pizza timer number, must be between 0 and MAX_KEYS
 */
static inline uint8_t pizzatimer_running(uint8_t n)
{
	return isKeyTimerRunning(MAX_KEYS + n);
}

/**
 * helper to clear a pizza timer.
 */
static inline void pizzatimer_clear(uint8_t n)
{
	clearKeyTimeout(MAX_KEYS + n);
}

#endif /* KEY_TIMER_H_ */
