#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "mash_a.h"
#include "acnh_menu.h"
#include "sleep.h"
#include "main_menu.h"

const char PROGMEM routine_main_menu_name[] = "Main Menu";

Routine *routine_main_menu_members[] = {
	&routine_mash_a,
	&routine_acnh_menu,
	&routine_sleep,
};

Routine routine_main_menu = {
	routine_main_menu_name,
	NULL,
	NULL,
	routine_main_menu_members,
	3,
	0,
	false,
};
