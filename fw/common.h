#ifndef COMMON_H_
#define COMMON_H_

#include "hw.h"

enum events {
	EV_NONE         = 0,
	EV_ENCODER_CW,
	EV_ENCODER_CCW,
	EV_ENCODER_PUSH,
	EV_SMAUL_PUSH,
	EV_TICK,
	EV_KEY_CHANGE0,
	EV_KEY_CHANGE_LAST = EV_KEY_CHANGE0 + MAX_KEYS - 1
};

#define EVENT_QUEUE_SIZE 8
#define NAME_LENGTH 16

typedef char name_t[NAME_LENGTH + 1];

#define __STRINGIFY2__(x) #x
#define __STRINGIFY__(x) __STRINGIFY2__(x)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

void push_event(uint8_t event);
uint8_t get_event(void);

/* Global timers, counting milliseconds and quarter-seconds */
extern volatile uint8_t global_ms_timer;
extern volatile uint8_t global_qs_timer;

void call_bootloader(void) __attribute__((noreturn));

#endif /* COMMON_H_ */
