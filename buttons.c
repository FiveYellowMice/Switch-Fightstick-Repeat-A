#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"

uint8_t buttons_state = 0x00;

void buttons_setup(void) {
	// Set up button pins to INPUT_PULLUP
	DDRB &= ~_BV(PB4); // D8, btn_left
	PORTB |= _BV(PB4);
	DDRB &= ~_BV(PB5); // D9, btn_return
	PORTB |= _BV(PB5);
	DDRB &= ~_BV(PB2); // D16, btn_play_pause
	PORTB |= _BV(PB2);
	DDRB &= ~_BV(PB3); // D14, btn_right
	PORTB |= _BV(PB3);

	// Enable pin change interrupt on btn_left for waking up
	PCMSK0 |= _BV(PCINT4);
	PCICR |= _BV(PCIE0);
}

EMPTY_INTERRUPT(PCINT0_vect);

void buttons_debounce(void) {
	buttons_state = 0x00;

	// https://www.avrfreaks.net/sites/default/files/forum_attachments/debounce.pdf
	static uint8_t count_low = 0xFF, count_high = 0xFF;
	static uint8_t last_state = 0xFF;

	uint8_t current_state =
		(!(PINB & _BV(PB4)) << 3) |
		(!(PINB & _BV(PB5)) << 2) |
		(!(PINB & _BV(PB2)) << 1) |
		!(PINB & _BV(PB3));

	uint8_t state_changed = current_state ^ last_state;

	count_low = ~(count_low & state_changed);
	count_high = count_low ^ (count_high & state_changed);

	state_changed &= count_low & count_high;

	last_state ^= state_changed;
	buttons_state |= last_state & state_changed;
}
