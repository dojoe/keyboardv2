#ifndef COMMON_H_
#define COMMON_H_

/* Use to enable some debug features, like jumping to the bootloader from the front panel menu */
#define DEBUG 0

#if DEBUG
#define DEBUG_VERSION "-debug"
#else
#define DEBUG_VERSION
#endif
#define FW_VERSION "0.91" DEBUG_VERSION

#include "hw.h"

enum events {
	EV_NONE         = 0,
	EV_ENCODER_CW,
	EV_ENCODER_CCW,
	EV_ENCODER_PUSH,
	EV_SMAUL_PUSH,
	EV_TICK,
	EV_KEY_CHANGE,
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

static inline uint8_t in_test_mode(void) {
	extern uint8_t g_test_mode;
	return g_test_mode;
}

#define WDR_RESET      0
#define WDR_BOOTLOADER 1
#define WDR_TESTMODE   2

void watchdog_reset(uint8_t where) __attribute__((noreturn));

static inline void reset_system(void)
{
	watchdog_reset(WDR_RESET);
}

static inline void call_bootloader(void)
{
	watchdog_reset(WDR_BOOTLOADER);
}

static inline void enter_test_mode(void)
{
	watchdog_reset(WDR_TESTMODE);
}

#endif /* COMMON_H_ */
