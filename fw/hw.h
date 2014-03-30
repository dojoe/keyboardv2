#ifndef HW_H_
#define HW_H_

#ifndef __NO_INCLUDE_AVR
#include <avr/io.h>
#include <avr/pgmspace.h>
#else 
#include <stdint.h>
#define PSTR(str) str
#endif // __NO_INCLUDE_AVR

// Access bits like variables
struct bits {
  uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} __attribute__((__packed__));
#define SBIT_(port,pin) ((*(volatile struct bits*)&port).b##pin)
#define	SBIT(x,y)       SBIT_(x,y)

#define SHIFTREG_LATCH SBIT(PORTF, 0)

#define MAX_KEYS 8

#define LCD_LED   OCR1B
#define SMAUL_LED OCR1A

#define PWR_PORT  PORTD
#define PWR_PIN   PIND
#define PWR_DDR   DDRD
#define PWR_BIT   (1 << PD0)

#define IN_ROTA  (1 << PB7)
#define IN_ROTB  (1 << PB3)
#define IN_SMAUL (1 << PE2)
#define IN_PUSH  (1 << PB4)

struct shiftregs {
	uint8_t key_sel:3, key_en:1, blaulicht:1, beeper:1, nc:2;
	uint8_t leds;
};

#define min(x,y)  ((x)<(y) ? (x) : (y))
#define max(x,y)  ((x)>(y) ? (x) : (y))

#ifndef __NO_INCLUDE_AVR
#include "panel.h"
#endif // __NO_INCLUDE_AVR


// Magic Constnats:

#define NUM_PIZZA_TIMERS 3

#endif /* HW_H_ */
