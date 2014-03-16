/** 
 * key_timer.h
 * definitions for the timer stuff.
 */

#ifndef KEY_TIMER_H_
#define KEY_TIMER_H_

#define SCHLUESSELTIMER_STATE_IDLE	0

void initTimers(void);
void key_change(uint8_t slot);

#endif /* KEY_TIMER_H_ */
