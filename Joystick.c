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

#include "Joystick.h"


uint16_t fruits_bought = 0;

int main(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	DDRD &= ~_BV(PD0);
	PORTD |= _BV(PD0); // Enable internal pullup for SEL button
	DDRD |= _BV(PD4);
	PORTD &= ~_BV(PD4); // Set PD4 to ground in order to use OK button
	DDRD &= ~_BV(PD7);
	PORTD |= _BV(PD7); // Enable internal pullup for OK button

	DDRB |= _BV(PB0); // Left LED
	DDRD |= _BV(PD5); // Right LED

	// Enable internal clock with scaler 64
	TCCR0B |= _BV(CS00) | _BV(CS01);
	// Initialize TIMER0
	TCNT0 = 0;
	TIMSK0 |= _BV(TOIE0);
	sei();

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);
	// The USB stack should be initialized last.
	//USB_Init();

	GlobalInterruptEnable();

	for (;;) {
		// Stop if the OK button is pressed.
		//if (READ_BTN_OK()) {
		//	SET_LED_R(false);
		//	USB_Disable();
		//	while (1);
		//}

		// Update button states
		bool btn_sel_current_state = !(PIND & _BV(PD0));
		bool btn_ok_current_state = !(PIND & _BV(PD7));
		btn_sel_detected = btn_sel_current_state && !btn_sel_last_state;
		btn_ok_detected = btn_ok_current_state && !btn_ok_last_state;
		btn_sel_last_state = btn_sel_current_state;
		btn_ok_last_state = btn_ok_current_state;

		if (state == STANDBY) {

			// Blink left LED to indicate selected mode
			switch (mode) {
				case FAST:
					SET_LED_L(milliseconds % 100 < 50);
					break;
				case SLOW:
					SET_LED_L(milliseconds % 500 < 50);
					break;
				case NOOKS_CRANNY_BULK_BUY:
					SET_LED_L(milliseconds % 600 < 50 || (milliseconds % 600 >= 100 && milliseconds % 600 < 150));
					break;
				case METEOR:
					SET_LED_L((milliseconds % 1500 >= 500 && milliseconds % 1500 < 550) || milliseconds % 1500 >= 1000);
					break;
				case OFF:
					break;
			}

			// Switch selected modes
			if (btn_sel_detected) {
				SET_LED_L(false);
				if (mode == OFF) {
					mode = 0;
				} else {
					mode++;
				}
			}
			if (btn_ok_detected && mode != OFF) {
				// Start running
				SET_LED_L(false);
				USB_Init();
				fruits_bought = 0;
				milliseconds = 0;
				state = SYNC_CONTROLLER;
			}

		} else if (state == STOPPED) {
			// Stop running
			SET_LED_R(false);
			USB_Disable();
			milliseconds = 0;
			state = STANDBY;

		} else {
			if (btn_ok_detected) {
				state = STOPPED;
			}
		}
		
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Increment milliseconds counter per 1.024 milliseconds.
ISR(TIMER0_OVF_vect) {
	milliseconds = (milliseconds + 1) % 60000;
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
	if (USB_DeviceState != DEVICE_STATE_Configured)
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
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
		GetNextReport(&JoystickInputData);
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

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
				break;
			default:
				;
		}
	}

	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;

}
