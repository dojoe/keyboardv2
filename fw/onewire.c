#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "onewire.h"

uint8_t ow_buffer[OW_MAX_XFER];
volatile enum ow_state_e ow_state;

static uint8_t phase, step, shiftreg, next_bit;
static uint8_t ow_ptr, ow_read_pos, ow_last_pos;
static uint8_t *ow_read_buf;

static inline void set_tx_pin(int value)
{
	if (value)
		PORTD |= 1 << PD0;
	else
		PORTD &= ~(1 << PD0);
}

static inline int get_rx_pin(void)
{
	return !!(PIND & (1 << PD0));
}

static inline void set_txrx(int tx)
{
	if (tx) {
		DDRD |= 1 << PD0;
	} else {
		DDRD &= ~(1 << PD0);
		PORTD |= 1 << PD0;
	}
}

ISR(TIMER0_COMPA_vect)
{
	/* Get a stable local copy of the volatile control variable */
	enum ow_state_e local_state = ow_state;

	if (phase == 0) PORTD ^= 1 << PD1;

	/* First of all, do the actual bit TX/RX to minimize jitter */
	if (local_state == OW_TX) {
		if (phase == 0) {
			set_txrx(1);
			set_tx_pin(!next_bit);
		} else if (phase == 2) {
			set_tx_pin(next_bit);
		}
	} else if (local_state == OW_RX) {
		if (phase == 0)
			set_txrx(0);
		else if (phase == 1)
			next_bit = get_rx_pin();
		else if (phase == 3)
			next_bit = get_rx_pin() && !next_bit;
	} else { /* IDLE or ERROR */
		return;
	}

	/* bookkeeping, bit shifting and stuff */
	phase = (phase + 1) & 3;
	if (phase == 0) {
		step++;
		if (step < 8) {
			if (local_state == OW_TX) {
				next_bit = !!(shiftreg & 0x80);
			} else { /* OW_RX */
				shiftreg |= next_bit;
			}
			shiftreg <<= 1;
		} else if (step == 8) {
			if (local_state == OW_RX)
				shiftreg |= next_bit;
			next_bit = !(ow_ptr == ow_last_pos);
			ow_state = OW_TX;
		} else if (step == 9) {
			ow_state = OW_RX;
		} else {
			if (!next_bit && ow_ptr) { /* NoSAK on anything other than the sync byte */
				ow_state = OW_ERROR;
				return;
			}

			step = 0;

			if (ow_ptr >= ow_read_pos)
				*(ow_read_buf++) = shiftreg;

			ow_ptr++;

			if (ow_ptr > ow_last_pos) {
				set_txrx(1);
				set_tx_pin(1);
				ow_state = OW_IDLE;
			} else if (ow_ptr >= ow_read_pos) {
				ow_state = OW_RX;
			} else {
				ow_state = OW_TX;
				shiftreg = ow_buffer[ow_ptr];
				next_bit = !!(shiftreg & 0x80);
			}
			shiftreg <<= 1;
		}
	}
}

void ow_init(void)
{
	ow_disconnect(); /* Force sane state */

	/*
	 * Set up Timer 0 to run at F_CPU / 8 (0.5us per tick).
	 * Overflow after 25 ticks -> Interrupt every 12.5us -> 50us bit period -> 20kHz bus frequency
	 */
	OCR0A  = 24;
	TCNT0  = 0;
	TIMSK0 = 1 << OCIE0A;
	TIFR0  = 1 << OCF0A;
	TCCR0A = 2 << WGM00; /* CTC mode */
	TCCR0B = 2 << CS00;  /* prescaler divide by 8 & start timer */
}

void ow_reset(void)
{
	set_txrx(1);
	set_tx_pin(0);   /* Datasheet requires a 0->1 transition after POR/BOR; make sure there is one */
	_delay_us(5);    /* Give the line time to go low */
	set_tx_pin(1);
	_delay_us(600);  /* Datashet table 1-2 says Tstby > 600us */
	ow_state = OW_IDLE;
}

void ow_start(uint8_t write_size, uint8_t read_size, void *read_buf)
{
	if (ow_state == OW_ERROR)
		ow_reset();

	phase = 0;
	step = 0;
	ow_ptr = 0;
	ow_last_pos = write_size + read_size - 1;
	ow_read_pos = read_size ? write_size : 0xFF;
	ow_read_buf = read_buf ?: ow_buffer + write_size;
	shiftreg = 0x55 << 1;
	next_bit = 0;

	/* send start sequence */
	set_txrx(1);
	set_tx_pin(1);
	_delay_us(15);  /* Datasheet table 1-2 says Tss > 10us */
	set_tx_pin(0);
	_delay_us(7);	/* Datasheet table 1-2 says Thdr > 5us */

	/* Set off timer function */
	ow_state = OW_TX;
}

void ow_disconnect(void)
{
	set_txrx(1);
	set_tx_pin(0);
	ow_state = OW_ERROR;  /* Make sure the first transmission starts with a reset */
}

uint8_t ow_wait(void)
{
	sleep_enable();
	while (!ow_done());
		sleep_cpu();
	sleep_disable();
	return !ow_error();
}
