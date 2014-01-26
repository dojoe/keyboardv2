#ifndef PANEL_H_
#define PANEL_H_

#include "hw.h"

extern struct shiftregs shiftregs;
void shiftreg_reset(void);
void shiftreg_update(void);

static inline void set_lcd_led(uint8_t value)
{
	LCD_LED = 255 - value;
}

static inline void set_smaul_led(uint8_t value)
{
	SMAUL_LED = 255 - value;
}

enum input_events {
	IN_NONE         = 0,
	IN_ENCODER_CW,
	IN_ENCODER_CCW,
	IN_ENCODER_PUSH,
	IN_SMAUL_PUSH
};

uint8_t next_input_event(void);

enum beep_patterns {
	BEEP_OFF = 0,
	BEEP_SINGLE,
	BEEP_KEYMISSING,
	BEEP_ERROR,
	BEEP_PIZZA1,
	BEEP_PIZZA2,
	BEEP_PIZZA3,
};

void beeper_start(uint8_t pattern);
static inline void beeper_stop(void) {
	beeper_start(BEEP_OFF);
}

#define LCD_LED_DIM  13
#define LCD_LED_ON   255
#define LCD_LED_UP   42
#define LCD_LED_DOWN 3

void set_lcd_backlight(uint8_t on);

void smaul_pulse(uint8_t frequency);
void smaul_blink(uint8_t frequency);
void smaul_off(void);

void keyled_on(uint8_t which);
void keyled_blink(uint8_t which);
void keyleds_off(void);

#endif /* PANEL_H_ */
