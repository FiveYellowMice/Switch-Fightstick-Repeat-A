#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "SSD1306.h"
#include "main_menu.h"
#include "sleep.h"

void routine_sleep_function(Routine_Flags *routine_flags) {
	display_draw_text(0, 1, PSTR("Sleep"), false);
}

DEFINE_ROUTINE_FUNCTION(sleep, "Sleep", main_menu)
