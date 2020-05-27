#define SD1306_I2C_ADDR 0x3C
#define DISPLAY_BUFFER_SIZE (128 * 64 / 8)

extern uint8_t display_buffer[DISPLAY_BUFFER_SIZE];

int SSD1306_setup(void);
int SSD1306_display(void);
int SSD1306_command_list(uint8_t* data, size_t data_size);
