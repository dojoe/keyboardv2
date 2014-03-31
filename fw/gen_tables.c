#include <stdio.h>
#include <math.h>

/* Generate gamma table for smaul LED driver */
void print_gamma_table(void)
{
	int i;
	double x, y;
	printf("static const PROGMEM uint8_t gamma[64] = {\n\t");
	for (i = 0; i < 64; i++) {
		x = (double)i / 63.0;
		y = pow(x, 2.2);
		printf("%i, ", (int)floor((y * 255.0) + 0.5));
	}
	printf("\n};\n\n");
}

/*
 * The pulsation frequency of the Smaul button LED depends on how soon the Keyboard will
 * start to throw a fit. As frequency is not perceived linearly, but rather logarithmically
 * (see audio frequencies), we want to double the frequency roughly every time unit, but
 * preferably we want to go slower in the beginning and go faster the closer to the alarm we
 * get. So let's choose
 *   freq = 2^y  with  y = a*(x^2) + b  and  x = t0 - t
 *   and a, b chosen such that  freq(0) = 200  and  freq(t0) = 6
 *
 * 2^b = 6 <=> b = log2(6)
 * 2^(a*t0^2 + b) = 200 <=> a = (log2(200) - log(6)) / (t0^2)
 *
 * We avoid floating point math in the uC by generating a small table for these values
 * and interpolating between them.
 */

const double t0 = 180.0;
const double f0 = 6.0;
const double f1 = 200.0;

const int INTERP_FACTOR = 8;

double log2(double x)
{
	return log(x) / log(2);
}

void print_smaul_table(void)
{
	int i;
	double x, y, a, b;
	int TABLE_SIZE = (int)t0 / INTERP_FACTOR;

	b = log2(f0);
	a = (log2(f1) - log2(f0)) / (t0 * t0);

	printf("static const PROGMEM uint8_t smaul_freq[] = {\n\t");
	for (i = 0; i < TABLE_SIZE; i++) {
		x = (1.0 - (double)i / (double)(TABLE_SIZE - 1)) * t0;
		y = pow(2, a * x * x + b);
		printf("%i, ", (int)floor(y + 0.5));
	}
	printf("\n};\n\n");
}

int main(void)
{
	print_gamma_table();
	print_smaul_table();
	return 0;
}
