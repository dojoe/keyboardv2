#ifndef PANEL_H_
#define PANEL_H_

#ifndef __NO_INCLUDE_AVR
#include "hw.h"
#endif // __NO_INCLUDE_AVR

#define LCD_BACKLIGHT_TIMEOUT_SECS 30

extern struct shiftregs shiftregs;
void shiftreg_update(void);

#ifndef __NO_INCLUDE_AVR
static inline uint8_t shiftreg_done(void)
{
	extern uint8_t shiftreg_state;
	return (shiftreg_state >= 2);
}

static inline void set_lcd_led(uint8_t value)
{
	LCD_LED = 255 - value;
}

static inline void set_smaul_led(uint8_t value)
{
	SMAUL_LED = 255 - value;
}
#endif // __NO_INCLUDE_AVR

enum beep_patterns {
	BEEP_OFF = 0,
	BEEP_SINGLE,
	BEEP_KEYMISSING,
	BEEP_ERROR,
	BEEP_PIZZA1,
	BEEP_PIZZA2,
	BEEP_PIZZA3,
	BEEP_DISABLED,
};

void beeper_start(uint8_t pattern);
static inline void beeper_stop(void) {
	beeper_start(BEEP_OFF);
}
void beeper_enable(uint8_t enable);

#define LCD_LED_DIM  13
#define LCD_LED_ON   255
#define LCD_LED_UP   42
#define LCD_LED_DOWN 3

void enable_lcd_backlight(void);

void smaul_pulse(uint8_t frequency);
void smaul_blink(uint8_t frequency);
void smaul_off(void);

void keyled_on(uint8_t which);
void keyled_blink(uint8_t which);
void keyleds_off(void);

void lcd_printfP(uint8_t line, const char *fmt, ...);
void lcd_print_start(uint8_t line);
void lcd_print_update_P(uint8_t line, const char *fmt, ...);
void lcd_print_end(uint8_t line);
void lcd_poll(void);

void panel_init(void);

#endif /* PANEL_H_ */
