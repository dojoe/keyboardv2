#ifndef HW_H_
#define HW_H_

#include <avr/io.h>

// Access bits like variables
struct bits {
  uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} __attribute__((__packed__));
#define SBIT_(port,pin) ((*(volatile struct bits*)&port).b##pin)
#define	SBIT(x,y)       SBIT_(x,y)

#define SHIFTREG_LATCH SBIT(PORTF, 0)

#define LCD_LED   OCR1B
#define SMAUL_LED OCR1A

#define IN_ROTA  (1 << PB7)
#define IN_ROTB  (1 << PB3)
#define IN_SMAUL (1 << PE2)
#define IN_PUSH  (1 << PB4)

struct shiftregs {
	uint8_t key_sel:3, key_en:1, blaulicht:1, buzzer:1, nc:2;
	uint8_t leds;
};

#endif /* HW_H_ */
