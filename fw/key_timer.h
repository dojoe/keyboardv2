/** 
 * key_timer.h
 * definitions for the timer stuff.
 */

#ifndef KEY_TIMER_H_
#define KEY_TIMER_H_

#define SCHLUESSELTIMER_STATE_IDLE	0

void lcd_resettimer(void);
void lcd_timeout(void);
void initTimers(void);

#endif /* KEY_TIMER_H_ */
