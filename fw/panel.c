#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw.h"
#include "panel.h"

struct shiftregs shiftregs = {
	0, 1, 0, 0, 0, 0
};

#define shiftreg_bytes ((uint8_t *)&shiftregs)

uint8_t shiftreg_state;

ISR(SPI_STC_vect)
{
	shiftreg_state++;
	if (shiftreg_state < 2)
		SPDR = shiftreg_bytes[shiftreg_state];
	else {
		SHIFTREG_LATCH = 1;
		SPCR = 0;
		SHIFTREG_LATCH = 0;
	}
}

#define SPI_SETTINGS (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (2 << SPR0)

void shiftreg_reset(void)
{
	uint8_t dummy __attribute__((unused));

	SPSR = 0;
	SPCR = SPI_SETTINGS;
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	dummy = SPDR; // read SPDR to clear SPIF
	SHIFTREG_LATCH = 1;
	SPCR = 0;
	SHIFTREG_LATCH = 0;
}

void shiftreg_update(void)
{
	SPSR = 0;
	SPCR = (1 << SPIE) | SPI_SETTINGS;
	shiftreg_state = 0;
	SPDR = shiftreg_bytes[0];
}

uint8_t inputs_prev = 0, inputs_debounced = 0, inputs_debounced_prev = 0;

#define EVENT_QUEUE_SIZE 8

uint8_t event_queue[EVENT_QUEUE_SIZE];
volatile uint8_t event_queue_head = 0, event_queue_tail = 0;

#define IN_MASKB (IN_ROTA | IN_ROTB | IN_PUSH)
#define IN_MASKE IN_SMAUL

static inline uint8_t event_queue_full(void)
{
	return ((event_queue_head - event_queue_tail) == EVENT_QUEUE_SIZE);
}

static inline uint8_t event_queue_empty(void)
{
	return (event_queue_head == event_queue_tail);
}

static inline void push_event(uint8_t event)
{
	if (event_queue_full())
		return; // drop event

	event_queue[(event_queue_head++) & (EVENT_QUEUE_SIZE - 1)] = event;
}

uint8_t next_input_event(void)
{
	return event_queue_empty() ? IN_NONE : event_queue[(event_queue_tail++) & (EVENT_QUEUE_SIZE - 1)];
}

void poll_inputs(void)
{
	uint8_t inputs = (PINB & IN_MASKB) | (PINE & IN_MASKE);
	uint8_t debounce_low = inputs | inputs_prev;
	uint8_t debounce_high = inputs & inputs_prev;
	inputs_prev = inputs;

	inputs_debounced |= debounce_high;
	inputs_debounced &= debounce_low;

	/* Here's the trick for the rotary encoder:
	 *
	 * Due to the detents, the first half of the quadrature cycle (where you have to invest force to overcome the current detent)
	 * takes way longer than the second half (where the knob snaps into the next detent without outside help).
	 *
	 * The result is that depending on direction, the signal edges on either A or B are further apart than on the other signal.
	 * So when we look for an edge, we're better off checking for edges on both signals instead of just looking for the edge on
	 * one signal and deriving the direction from the other signal.
	 */
	if ((inputs_debounced_prev & IN_ROTA) && !(inputs_debounced & IN_ROTA) && (inputs_debounced_prev & IN_ROTB))
		push_event(IN_ENCODER_CW);
	else if ((inputs_debounced_prev & IN_ROTB) && !(inputs_debounced & IN_ROTB) && (inputs_debounced_prev & IN_ROTA))
		push_event(IN_ENCODER_CCW);

	if ((inputs_debounced_prev & IN_PUSH) && !(inputs_debounced & IN_PUSH))
		push_event(IN_ENCODER_PUSH);

	if ((inputs_debounced_prev & IN_SMAUL) && !(inputs_debounced & IN_SMAUL))
		push_event(IN_SMAUL_PUSH);

	inputs_debounced_prev = inputs_debounced;
}
