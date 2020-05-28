#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "main_menu.h"
#include "sleep.h"

const char PROGMEM routine_sleep_name[] = "Sleep";

void routine_sleep_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("Sleep"), false);
}

Routine routine_sleep = {
	routine_sleep_name,
	&routine_main_menu,
	routine_sleep_function,
	NULL,
	0,
	0,
	false,
};
