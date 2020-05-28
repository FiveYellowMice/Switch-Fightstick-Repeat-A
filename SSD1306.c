#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Drivers/Peripheral/TWI.h>

#include "font.h"
#include "SSD1306.h"

uint8_t display_buffer[DISPLAY_BUFFER_SIZE];

int SSD1306_setup(void) {
	TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 800000L));

	// Clear buffer
	memset(&display_buffer, 0, DISPLAY_BUFFER_SIZE);

	uint8_t init_sequence1[] = {
		0xAE, // Display OFF
		0xD5, 0x80, // Set display clock div
		0xA8, 0x3F, // Set multiplex
		0xD3, 0x00, // Set display offset to 0
		0x40 | 0x00, // Set start line to 0
		0x8D, 0x14, // Enable charge pump
	};
	if (SSD1306_command_list(init_sequence1, sizeof(init_sequence1)) != 0) return -1;
	uint8_t init_sequence2[] = {
		0x20, 0x00, // Set memory mode to horizontal addressing
		0xA0 | 0x01, // Set segment remap
		0xC8, // Set COM scan mode to remapped
		0xDA, 0x12, // Set COM pins hardware configuration to alternative
		0x81, 0x00, // Set contrast to lowest
		0xD9, 0xF1, // Set precharge period, phase 1 to 15 DCLK, phase 2 to 1 DCLK
		0xDB, 0x40, // Set Vcomh deselect level to unknown
		0xA4, // Display resume
		0xA6, // Set normal display
		0x2E, // Deactivate scroll
		0xAF, // Display ON
	};
	if (SSD1306_command_list(init_sequence2, sizeof(init_sequence2)) != 0) return -1;

	return 0;
}

int SSD1306_display(void) {
	uint8_t pre_display_sequence[] = {
		0x22, 0x00, 0xFF, // Set page address
		0x21, 0x00, 0x7F, // Set column address
	};
	if (SSD1306_command_list(pre_display_sequence, sizeof(pre_display_sequence)) != 0) return -1;

	for (uint8_t i = 0; i < DISPLAY_BUFFER_SIZE / 16; i++) {
		if (TWI_StartTransmission(SD1306_I2C_ADDR << 1, 10) != TWI_ERROR_NoError) return -1;
		if (!TWI_SendByte(0x40)) return -1; // Control byte, Co = 0, D/C = 1
		for (uint8_t j = 0; j < 16; j++) {
			if (!TWI_SendByte(display_buffer[i * 16 + j])) return -1;
		}
		TWI_StopTransmission();
	}

	return 0;
}

int SSD1306_command_list(uint8_t* data, size_t data_size) {
	if (data_size > 16) return -1;

	if (TWI_StartTransmission(SD1306_I2C_ADDR << 1, 10) != TWI_ERROR_NoError) return -1;

	if (!TWI_SendByte(0x00)) return -1; // Control byte, Co = 0, D/C = 0

	for (uint16_t i = 0; i < data_size; i++) {
		if (!TWI_SendByte(data[i])) return -1; // Command byte
	}

	TWI_StopTransmission();
	return 0;
}

void display_set_pixel(int16_t x, int16_t y, bool c) {
	if (x < 0 || x >= 128 || y < 0 || y >= 64) {
		// Outside of screen
		return;
	}

	uint16_t byte_index = y / 8 * 128 + x;
	uint8_t bit_index = y % 8;

	if (c) {
		display_buffer[byte_index] |= (1 << bit_index);
	} else {
		display_buffer[byte_index] &= ~(1 << bit_index);
	}
}

void display_clear(void) {
	memset(&display_buffer, 0, DISPLAY_BUFFER_SIZE);
}

void display_fill_line(int16_t x, int8_t line, uint8_t length, bool c) {
	if (x < 0 || x + length - 1 >= 128 || line < 0 || line >= 4) {
		// Outside of screen
		return;
	}

	for (uint8_t i = 0; i < length; i++) {
		display_buffer[line * 2 * 128 + x + i] = c ? 0xFF : 0x00;
		display_buffer[(line * 2 + 1) * 128 + x + i] = c ? 0xFF : 0x00;
	}
}

void display_draw_glyph(int16_t x, int8_t line, const uint16_t *glyph, uint8_t glyph_width) {
	if (x <= -glyph_width || x >= 128 || line < 0 || line >= 4) {
		// Outside of screen
		return;
	}

	for (uint8_t i = 0; i < glyph_width && x + i < 128; i++) {
		uint16_t column = pgm_read_word(glyph + i);
		display_buffer[line * 2 * 128 + x + i] ^= (uint8_t) column;
		display_buffer[(line * 2 + 1) * 128 + x + i] ^= (uint8_t) (column >> 8);
	}
}

void display_draw_text(int16_t x, int8_t line, const char *s, bool is_ram) {
	char c;
	uint8_t i = 0;
	while (1) {
		if (is_ram) {
			c = s[i];
		} else {
			c = pgm_read_byte(s + i);
		}
		if (c == '\0') break;

		if (c >= ' ' && c <= '~') {
			display_draw_glyph(x + i * GLYPH_WIDTH_CHAR, line, font + (c - 0x20) * GLYPH_WIDTH_CHAR, GLYPH_WIDTH_CHAR);
		}
		i++;
	}
}
