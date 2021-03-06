/*
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/

/** \file
 *
 *  Main source file for the posts printer demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Platform/Platform.h>

#include "Descriptors.h"
#include "font.h"
#include "SSD1306.h"
#include "buttons.h"
#include "routine.h"
#include "routines/main_menu.h"
#include "Joystick.h"


volatile milliseconds_t milliseconds = 0;
bool sleep = true; // Enter sleep mode when powered on
Routine *current_routine = NULL;

#define USB_ECHOES 2
USB_JoystickReport_Input_t usb_report;
USB_JoystickReport_Input_t usb_last_report;

int main(void) {
	// Shut down modules we don't use: Timer1, Timer3, SPI, ADC, USART1
	PRR0 |= _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC);
	PRR1 |= _BV(PRTIM3) | _BV(PRUSART1);

	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	DDRB |= _BV(PB0); // Left LED
	DDRD |= _BV(PD5); // Right LED

	// Enable internal clock with scaler 64
	TCCR0B |= _BV(CS00) | _BV(CS01);
	// Initialize TIMER0
	TCNT0 = 0;
	TIMSK0 |= _BV(TOIE0);
	sei();

	buttons_setup();

	SET_LED_L(SSD1306_setup());

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);
	// The USB stack should be initialized last.
	//USB_Init();
	GlobalInterruptEnable();

	current_routine = &routine_main_menu;

	for (;;) {
		if (sleep) {
			// MCU sleep process
			SET_LED_L(SSD1306_power(false));
			if (USB_IsInitialized) USB_Disable();
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			cli();
			sleep_enable();
			sei();
			sleep_cpu();
			// Pressing btn_left (PCINT0) should wake the MCU up

			// After waking up
			sleep_disable();
			sleep = false;
			SET_LED_L(SSD1306_power(true));
		}

		buttons_debounce();

		if (USB_IsInitialized && USB_DeviceState == DEVICE_STATE_Configured) {
			// Clear USB report
			memset(&usb_report, 0, sizeof(USB_JoystickReport_Input_t));
			usb_report.LX = STICK_CENTER;
			usb_report.LY = STICK_CENTER;
			usb_report.RX = STICK_CENTER;
			usb_report.RY = STICK_CENTER;
			usb_report.HAT = HAT_CENTER;
		}

		// Prepare default routine flags
		Routine_Flags routine_flags = {0};
		routine_flags.returnable = routine_flags.indicator_return =  current_routine->upper_level != current_routine;

		// Draw the title
		display_draw_text(0, 0, current_routine->name, false);

		if (current_routine->function) {
			// Execute the routine if it has a function
			(current_routine->function)(&routine_flags);

		} else {
			// Menu
			routine_flags.indicator_left = true;
			routine_flags.indicator_right = true;

			uint8_t drawn_member_index = current_routine->menu_current_index;
			if (current_routine->menu_lower_selected) {
				drawn_member_index--;
				display_fill_line(0, 2, 128, true);
			} else {
				display_fill_line(0, 1, 128, true);
			}
			for (uint8_t i = 0; i < 2 && drawn_member_index + i < current_routine->menu_members_length; i++) {
				display_draw_text(0, 1 + i, current_routine->menu_members[drawn_member_index + i]->name, false);
			}

			if (BTN_STATE(BTN_LEFT)) {
				if (current_routine->menu_current_index == 0) {
					current_routine->menu_current_index = current_routine->menu_members_length - 1;
					if (current_routine->menu_members_length >= 2) {
						current_routine->menu_lower_selected = true;
					}
				} else {
					current_routine->menu_current_index--;
					if (current_routine->menu_lower_selected) {
						current_routine->menu_lower_selected = false;
					}
				}
			} else if (BTN_STATE(BTN_RIGHT)) {
				if (current_routine->menu_current_index == current_routine->menu_members_length - 1) {
					current_routine->menu_current_index = 0;
					if (current_routine->menu_members_length >= 2) {
						current_routine->menu_lower_selected = false;
					}
				} else {
					current_routine->menu_current_index++;
					if (!current_routine->menu_lower_selected) {
						current_routine->menu_lower_selected = true;
					}
				}
			} else if (BTN_STATE(BTN_PLAY_PAUSE)) {
				current_routine = current_routine->menu_members[current_routine->menu_current_index];
			}
		}

		if (!routine_flags.next_routine && routine_flags.returnable) {
			if (BTN_STATE(BTN_RETURN)) {
				routine_flags.next_routine = current_routine->upper_level;
			}
		}

		if (routine_flags.next_routine) {
			current_routine = routine_flags.next_routine;
		}

		// Draw indicators
		if (routine_flags.indicator_return) {
			display_draw_glyph(0, 3, symbol_return, 16);
		}
		if (USB_IsInitialized && USB_DeviceState == DEVICE_STATE_Configured) {
			display_draw_glyph(16, 3, symbol_usb, 16);
		}
		if (routine_flags.indicator_play_pause == PAUSE) {
			display_draw_glyph(32, 3, symbol_pause, 16);
		} else if (routine_flags.indicator_play_pause == PLAY) {
			display_draw_glyph(32, 3, symbol_play, 16);
		}
		if (routine_flags.indicator_left) {
			display_draw_glyph(112, 3, symbol_triangle_left, 8);
		}
		if (routine_flags.indicator_right) {
			display_draw_glyph(120, 3, symbol_triangle_right, 8);
		}

		// Refresh screen content and clear display buffer to redraw in the next cycle
		SET_LED_L(SSD1306_display());
		display_clear();

		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Increment milliseconds counter per 1.024 milliseconds.
ISR(TIMER0_OVF_vect) {
	milliseconds = milliseconds + 1;
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (!USB_IsInitialized || USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		static int echoes = 0;
		if (echoes > 0) {
			// Repeat USB_ECHOES times the last report
			memcpy(&usb_report, &usb_last_report, sizeof(USB_JoystickReport_Input_t));
			echoes--;
		} else {
			// Prepare to echo this report
			memcpy(&usb_last_report, &usb_report, sizeof(USB_JoystickReport_Input_t));
			echoes = USB_ECHOES;
		}
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&usb_report, sizeof(usb_report), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

/*
// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;

	// Repeat ECHOES times the last report
	if (echoes > 0)
	{
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}

	// States management
	if (state == SYNC_CONTROLLER) {
		if (milliseconds >= 6000) {
			milliseconds = 0;
			state = RUNNING;
		} else if (milliseconds >= 4200 && milliseconds < 4400) {
			ReportData->Button |= SWITCH_L | SWITCH_R;
		} else if (milliseconds >= 4800 && milliseconds < 5000) {
			ReportData->Button |= SWITCH_A;
		}

	} else {

		// Act depending on mode
		switch (mode) {
			case FAST:
				if (milliseconds % 100 < 50) {
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else {
					SET_LED_R(false);
				}
				break;
			case SLOW:
				if (milliseconds % 400 < 100) {
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else {
					SET_LED_R(false);
				}
				break;
			case NOOKS_CRANNY_BULK_BUY:
				if (milliseconds < 50) {
					// Select fruit
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else if (milliseconds >= 3000 && milliseconds < 3050) {
					// Advance dialog: Fruit? I can sell in single or in bulk. How many would you like?
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else if (milliseconds >= 3500 && milliseconds < 3550) {
					// Move cursor down to "I'll take 5!"
					ReportData->HAT = HAT_BOTTOM;
					SET_LED_R(true);
				} else if (milliseconds >= 3800 && milliseconds < 3850) {
					// Choose "I'll take 5!"
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else if (milliseconds >= 5800 && milliseconds < 5850) {
					// Advance dialog: Excellent purchase!
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else if (milliseconds >= 7800 && milliseconds < 7850) {
					// Advance dialog: Anything else look interesting?
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else if (milliseconds >= 8300) {
					fruits_bought += 5;
					if (fruits_bought >= 400) {
						state = STOPPED;
					} else {
						milliseconds = 0;
					}
				} else {
					SET_LED_R(false);
				}
				break;
			case METEOR:
				if (milliseconds % 10000 < 150) {
					ReportData->RY = STICK_MIN;
					SET_LED_R(true);
				} else if (milliseconds % 100 < 50) {
					ReportData->Button |= SWITCH_A;
					SET_LED_R(true);
				} else {
					SET_LED_R(false);
				}
				break;
			default:
				;
		}
	}

	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;

}
*/
