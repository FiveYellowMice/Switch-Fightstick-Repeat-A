#define SD1306_I2C_ADDR 0x3C
#define DISPLAY_BUFFER_SIZE (128 * 64 / 8)

int SSD1306_setup(void);
int SSD1306_display(void);
int SSD1306_command_list(uint8_t* data, size_t data_size);

void display_set_pixel(int16_t x, int16_t y, bool c);
void display_clear(void);

void display_fill_line(int16_t x, int8_t line, uint8_t length, bool c);
void display_draw_glyph(int16_t x, int8_t line, const uint16_t *glyph, uint8_t glyph_width);
void display_draw_text(int16_t x, int8_t line, const char *s, bool is_ram);
