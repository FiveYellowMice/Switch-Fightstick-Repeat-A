#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "../font.h"
#include "../SSD1306.h"
#include "../Joystick.h"
#include "main_menu.h"
#include "sleep.h"

void routine_sleep_function(Routine_Flags *routine_flags) {
	routine_flags->returnable = routine_flags->indicator_return = false;

	static bool waiting = false;
	static milliseconds_t wait_start_time = 0;

	if (!waiting) {
		waiting = true;
		wait_start_time = milliseconds;
	}

	// Wait for 1 second before actually sleeping
	if (milliseconds - wait_start_time >= 1000) {
		waiting = false;
		sleep = true;
		routine_flags->next_routine = &routine_main_menu;
		// Reset main menu position
		routine_main_menu.menu_current_index = 0;
		routine_main_menu.menu_lower_selected = false;
	} else {
		display_draw_text(43, 2, PSTR("Bye!"), false);
	}
}

DEFINE_ROUTINE_FUNCTION(sleep, "Sleep", main_menu)
