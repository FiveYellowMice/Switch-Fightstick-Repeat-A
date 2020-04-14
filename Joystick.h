/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for Joystick.c.
 */

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Platform/Platform.h>

#include "Descriptors.h"

volatile uint16_t milliseconds = 0;

// Macros for setting indicators
#define SET_LED_L(VAL1) ((VAL1) ? (PORTB &= ~_BV(PB0)) : (PORTB |= _BV(PB0)))
#define SET_LED_R(VAL1) ((VAL1) ? (PORTD &= ~_BV(PD5)) : (PORTD |= _BV(PD5)))

// Whether button was pressed in the last cycle
bool btn_sel_last_state = false;
bool btn_ok_last_state = false;
// Whether button has been pressed in this cycle but not the last
bool btn_sel_detected = false;
bool btn_ok_detected = false;

// Mode and state definitions
typedef enum {
	FAST,
	SLOW,
	NOOKS_CRANNY_BULK_BUY,
	METEOR,
	OFF,
} Mode_t;
Mode_t mode = FAST;

typedef enum {
	STANDBY,
	SYNC_CONTROLLER,
	RUNNING,
	STOPPED
} State_t;
State_t state = STANDBY;

// Type Defines
// Enumeration for joystick buttons.
typedef enum {
	SWITCH_Y       = 0x01,
	SWITCH_B       = 0x02,
	SWITCH_A       = 0x04,
	SWITCH_X       = 0x08,
	SWITCH_L       = 0x10,
	SWITCH_R       = 0x20,
	SWITCH_ZL      = 0x40,
	SWITCH_ZR      = 0x80,
	SWITCH_MINUS   = 0x100,
	SWITCH_PLUS    = 0x200,
	SWITCH_LCLICK  = 0x400,
	SWITCH_RCLICK  = 0x800,
	SWITCH_HOME    = 0x1000,
	SWITCH_CAPTURE = 0x2000,
} JoystickButtons_t;

#define HAT_TOP          0x00
#define HAT_TOP_RIGHT    0x01
#define HAT_RIGHT        0x02
#define HAT_BOTTOM_RIGHT 0x03
#define HAT_BOTTOM       0x04
#define HAT_BOTTOM_LEFT  0x05
#define HAT_LEFT         0x06
#define HAT_TOP_LEFT     0x07
#define HAT_CENTER       0x08

#define STICK_MIN      0
#define STICK_CENTER 128
#define STICK_MAX    255

// Joystick HID report structure. We have an input and an output.
typedef struct {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
	uint8_t  VendorSpec;
} USB_JoystickReport_Input_t;

// The output is structured as a mirror of the input.
// This is based on initial observations of the Pokken Controller.
typedef struct {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
} USB_JoystickReport_Output_t;

// Function Prototypes
// Process and deliver data from IN and OUT endpoints.
void HID_Task(void);
// USB device event handlers.
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData);

#endif
