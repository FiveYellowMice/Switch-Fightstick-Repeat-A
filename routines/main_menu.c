#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../routine.h"
#include "mash_a.h"
#include "acnh_menu.h"
#include "sleep.h"
#include "main_menu.h"

Routine *routine_main_menu_members[] = {
	&routine_mash_a,
	&routine_acnh_menu,
	&routine_sleep,
};



DEFINE_ROUTINE_MENU(main_menu, "Main Menu", main_menu, 3)
