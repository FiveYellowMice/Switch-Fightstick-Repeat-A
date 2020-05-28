#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "main_menu.h"
#include "mash_a.h"

const char PROGMEM routine_mash_a_name[] = "Mash A";

void routine_mash_a_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("AAAAAAA"), false);
}

Routine routine_mash_a = {
	routine_mash_a_name,
	&routine_main_menu,
	routine_mash_a_function,
	NULL,
	0,
	0,
	false,
};
