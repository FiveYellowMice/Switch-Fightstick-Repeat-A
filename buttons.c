#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"

uint8_t buttons_state = 0x00;

void buttons_setup(void) {
	// Set up button pins to INPUT_PULLUP
	DDRB &= ~_BV(PB4); // D8, btn_left
	PORTB |= _BV(PB4);
	DDRD &= ~_BV(PD2); // D0, btn_return
	PORTD |= _BV(PD2);
	DDRC &= ~_BV(PC6); // D5, btn_play_pause
	PORTC |= _BV(PC6);
	DDRD &= ~_BV(PD4); // D4, btn_right
	PORTD |= _BV(PD4);

	// Enable falling edge interrupt on btn_return for waking up
	EICRA |= _BV(ISC21);
	EIMSK |= _BV(INT2);
}

EMPTY_INTERRUPT(INT2_vect);

void buttons_debounce(void) {
	buttons_state = 0x00;

	// https://www.avrfreaks.net/sites/default/files/forum_attachments/debounce.pdf
	static uint8_t count_low = 0xFF, count_high = 0xFF;
	static uint8_t last_state = 0xFF;

	uint8_t current_state =
		(!(PINB & _BV(PB4)) << 3) |
		(!(PIND & _BV(PD2)) << 2) |
		(!(PINC & _BV(PC6)) << 1) |
		!(PIND & _BV(PD4));

	uint8_t state_changed = current_state ^ last_state;

	count_low = ~(count_low & state_changed);
	count_high = count_low ^ (count_high & state_changed);

	state_changed &= count_low & count_high;

	last_state ^= state_changed;
	buttons_state |= last_state & state_changed;
}
