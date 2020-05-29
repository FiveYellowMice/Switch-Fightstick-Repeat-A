#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "main_menu.h"
#include "mash_a.h"

void routine_mash_a_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("AAAAAAA"), false);
}

DEFINE_ROUTINE_FUNCTION(mash_a, "Mash A", main_menu)
