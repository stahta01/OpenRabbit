// "Hello, world!" on serial port A @ 19200 baud.

#include <stdint.h>
#include <stdio.h>

#include "r2k.h"

#if defined(RCM2020)
#define SERIAL_DIVIDER_19200 15

#elif defined(RCM2200)
#define SERIAL_DIVIDER_19200 18

#elif defined(RCM3110)
#define SERIAL_DIVIDER_19200 24

#elif defined(RCM3209)
#define SERIAL_DIVIDER_19200 36

#elif defined(RCM3319)
#define SERIAL_DIVIDER_19200 36

#elif defined(RCM3750)
#define SERIAL_DIVIDER_19200 18

#elif defined(RCM4110)
#define SERIAL_DIVIDER_19200 48 // 29.5 MHz crystal, thus clock running at full speed despite clock doubler off on startup.

#endif

// ___sdcc_external_startup, if present, will be called very early, before initalization
// of global objects. This makes it e.g. useful for dealing with watchdogs that might
// otherwise bite if there are many or large global objects that take a long time to initialize.
#if __SDCC_REVISION >= 13762
unsigned char __sdcc_external_startup(void)
#else
unsigned char _sdcc_external_startup(void)
#endif
{
	// Disable watchdog
	WDTTR = 0x51;
	WDTTR = 0x54;

	// normal oscillator, processor and peripheral from main clock, no periodic interrupt
	GCSR = 0x08;

	return 0;
}

int putchar(int c)
{
	// Convert newline to CRLF
	if (c == '\n') {
		putchar('\r');
	}

	while (SASR & 0x04);	// Wait for empty transmitter data register
	SADR = c;
	return c;
}

void main(void)
{
	PCFR = 0x40;	// Use pin PC6 as TXA

	TAT4R = SERIAL_DIVIDER_19200 - 1;	// Value in register is one less than the divider used (e.g. a value of 0 will result in clock division by 1).
	TACSR = 0x01;	// Enable timer A

	SACR = 0x00;	// No interrupts, 8-bit async mode

	for (;;) {
		printf("Hello, world!\n");
	}
}

