#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "acnh_menu.h"
#include "acnh_stargazing.h"

const char PROGMEM routine_acnh_stargazing_name[] = "Stargazing";

void routine_acnh_stargazing_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("Kirakira~"), false);
}

Routine routine_acnh_stargazing = {
	routine_acnh_stargazing_name,
	&routine_acnh_menu,
	routine_acnh_stargazing_function,
	NULL,
	0,
	0,
	false,
};
