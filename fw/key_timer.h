/** 
 * key_timer.h
 * definitions for the timer stuff.
 */

#ifndef KEY_TIMER_H_
#define KEY_TIMER_H_

extern uint8_t expired_key;

void initTimers(void);
void key_change(void);
void keytimer_display_update(void);
uint8_t pizzatimer_running(uint8_t n);
void setKeyTimeout(uint8_t key, uint8_t minutes);
void clearKeyTimeout(uint8_t key);
void pizzatimer_clear(uint8_t n);
void key_smaul(void);
void key_timer(void);

#endif /* KEY_TIMER_H_ */
