#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "main_menu.h"
#include "acnh_stargazing.h"
#include "acnh_menu.h"

const char PROGMEM routine_acnh_menu_name[] = "ACNH";

Routine *routine_acnh_menu_members[] = {
	&routine_acnh_stargazing,
};

Routine routine_acnh_menu = {
	routine_acnh_menu_name,
	&routine_main_menu,
	NULL,
	routine_acnh_menu_members,
	1,
	0,
	false,
};
