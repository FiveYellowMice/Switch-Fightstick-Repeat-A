#
#             LUFA Library
#     Copyright (C) Dean Camera, 2014.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

# Set the MCU accordingly to your device (e.g. at90usb1286 for a Teensy 2.0++, or atmega16u2 for an Arduino UNO R3)
MCU          = atmega32u4
ARCH         = AVR8
DEVICE_PATH ?= /dev/ttyACM1
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = Joystick
SRC          = $(TARGET).c Descriptors.c SSD1306.c font.c buttons.c $(wildcard routines/*.c) $(LUFA_SRC_USB) $(LUFA_SRC_TWI)
LUFA_PATH    = LUFA/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =

# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

flash:
	avrdude -C/etc/avrdude.conf -v -p${MCU} -cavr109 -P${DEVICE_PATH} -b57600 -D -Uflash:w:Joystick.hex:i
