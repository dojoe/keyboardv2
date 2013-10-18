#include <avr/io.h>
#include <util/delay.h>

#define PWR_PORT  PORTD
#define PWR_PIN   PIND
#define PWR_DDR   DDRD
#define PWR_BIT   (1 << PD3)

uint8_t key_present(void)
{
	uint8_t i;

	/* Detect if plug fully inserted.
	 *
	 * If the plug is fully inserted, the tip switch is open, so Tip is no longer shorted to GND.
	 * Check by switching Tip to pull-up and reading whether it stays high.
	 * Incidentally, if for some reason Tip is shorted to Sleeve, it will look just like a closed switch.
	 * As an additional bonus, the onewire code pulls Ring to GND when disconnected
	 * so we can detect shorts between Ring and Tip, too.
	 */
	PWR_DDR   &= ~PWR_BIT;
	PWR_PORT  |=  PWR_BIT;
	_delay_ms(1);

	for (i = 0; i < 10; i++) {
		_delay_us(10);
		if (!(PWR_PIN & PWR_BIT))
			goto out;
	}

	return 1;

out:
	PWR_PORT  &= ~PWR_BIT;
	PWR_DDR   &= ~PWR_BIT;
	return 0;
}

void key_on(void)
{
	PWR_PORT |= PWR_BIT;
	PWR_DDR  |= PWR_BIT;
	_delay_ms(1);
}

void key_off(void)
{
	PWR_PORT &= ~PWR_BIT;
	PWR_DDR  |=  PWR_BIT;
	_delay_ms(1);
}
