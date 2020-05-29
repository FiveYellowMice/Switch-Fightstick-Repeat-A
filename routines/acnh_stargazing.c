#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "acnh_menu.h"
#include "acnh_stargazing.h"

void routine_acnh_stargazing_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("Kirakira~"), false);
}

DEFINE_ROUTINE_FUNCTION(acnh_stargazing, "Stargazing", acnh_menu)
