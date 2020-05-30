#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "../SSD1306.h"
#include "../Joystick.h"
#include "main_menu.h"
#include "sleep.h"

void routine_sleep_function(Routine_Flags *routine_flags) {
	routine_flags->returnable = routine_flags->indicator_return = false;
	sleep = true;
	routine_flags->next_routine = &routine_main_menu;
}

DEFINE_ROUTINE_FUNCTION(sleep, "Sleep", main_menu)
