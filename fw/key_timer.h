/** 
 * key_timer.h
 * definitions for the timer stuff.
 */

#ifndef KEY_TIMER_H_
#define KEY_TIMER_H_

extern uint8_t expired_key;
extern int16_t keyTimers[MAX_KEYS + NUM_PIZZA_TIMERS];

void initTimers(void);
void key_change(void);
void keytimer_display_update(void);
void key_smaul(void);
void key_timer(void);

static inline void setKeyTimeout(uint8_t key, uint8_t minutes)
{
	keyTimers[key] = minutes * 1;
}

static inline void clearKeyTimeout(uint8_t key)
{
	keyTimers[key] = -1;
}

static inline uint8_t isKeyTimerRunning(uint8_t key)
{
	return (keyTimers[key] != -1);
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
