#include <avr/io.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Drivers/Peripheral/TWI.h>

#include "SSD1306.h"

uint8_t display_buffer[DISPLAY_BUFFER_SIZE];

int SSD1306_setup(void) {
	TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 1000000L));

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
		0x81, 0xCF, // Set contrast
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
