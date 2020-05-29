#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "main_menu.h"
#include "acnh_stargazing.h"
#include "acnh_menu.h"

Routine *routine_acnh_menu_members[] = {
	&routine_acnh_stargazing,
};

DEFINE_ROUTINE_MENU(acnh_menu, "ACNH", main_menu, 1)
