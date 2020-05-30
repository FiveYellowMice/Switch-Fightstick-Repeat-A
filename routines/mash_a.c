#include <stddef.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>
#include "../routine.h"
#include "../font.h"
#include "../SSD1306.h"
#include "../buttons.h"
#include "../Joystick.h"
#include "main_menu.h"
#include "mash_a.h"

#define CYCLE_LENGTH 160

void routine_mash_a_function(Routine_Flags *routine_flags) {
	static bool running = false;
	static milliseconds_t cycle_start_time = 0;
	milliseconds_t current_time = milliseconds;
	milliseconds_t cycle_running_time = current_time - cycle_start_time;

	routine_flags->indicator_play_pause = running ? PLAY : PAUSE;

	if (running && USB_IsInitialized && USB_DeviceState == DEVICE_STATE_Configured) {
		if (cycle_running_time < CYCLE_LENGTH / 2) {
			usb_report.Button |= SWITCH_A;
			display_draw_glyph(56, 2, symbol_button_a, 16);
		} else if (cycle_running_time >= CYCLE_LENGTH / 2 && cycle_running_time < CYCLE_LENGTH) {
			// No button pressed
		} else {
			cycle_start_time += CYCLE_LENGTH;
		}
	} else {
		cycle_start_time = current_time;
	}

	if (BTN_STATE(BTN_PLAY_PAUSE)) {
		if (!USB_IsInitialized) {
			USB_Init();
		}
		running = !running;
	}
	if (BTN_STATE(BTN_RETURN)) {
		if (USB_IsInitialized) {
			USB_Disable();
		}
		running = false;
	}
}

DEFINE_ROUTINE_FUNCTION(mash_a, "Mash A", main_menu)
