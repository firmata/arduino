/*
  Firmata.h - Firmata library v2.5.4 - 2016-10-23
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef FirmataConstants_h
#define FirmataConstants_h

/* Version numbers for the Firmata library.
 * The firmware version will not always equal the protocol version going forward.
 * Query using the REPORT_FIRMWARE message.
 */
#define FIRMATA_FIRMWARE_MAJOR_VERSION  2
#define FIRMATA_FIRMWARE_MINOR_VERSION  5
#define FIRMATA_FIRMWARE_BUGFIX_VERSION 4

/* Version numbers for the protocol.  The protocol is still changing, so these
 * version numbers are important.
 * Query using the REPORT_VERSION message.
 */
#define FIRMATA_PROTOCOL_MAJOR_VERSION  2 // for non-compatible changes
#define FIRMATA_PROTOCOL_MINOR_VERSION  5 // for backwards compatible changes
#define FIRMATA_PROTOCOL_BUGFIX_VERSION 1 // for bugfix releases

#ifdef MAX_DATA_BYTES
#undef MAX_DATA_BYTES
#endif
#define MAX_DATA_BYTES                  64 // max number of data bytes in incoming messages

// message command bytes (128-255/0x80-0xFF)

#ifdef DIGITAL_MESSAGE
#undef DIGITAL_MESSAGE
#endif
#define DIGITAL_MESSAGE         0x90 // send data for a digital port (collection of 8 pins)

#ifdef ANALOG_MESSAGE
#undef ANALOG_MESSAGE
#endif
#define ANALOG_MESSAGE          0xE0 // send data for an analog pin (or PWM)

#ifdef REPORT_ANALOG
#undef REPORT_ANALOG
#endif
#define REPORT_ANALOG           0xC0 // enable analog input by pin #

#ifdef REPORT_DIGITAL
#undef REPORT_DIGITAL
#endif
#define REPORT_DIGITAL          0xD0 // enable digital input by port pair

//

#ifdef SET_PIN_MODE
#undef SET_PIN_MODE
#endif
#define SET_PIN_MODE            0xF4 // set a pin to INPUT/OUTPUT/PWM/etc

#ifdef SET_DIGITAL_PIN_VALUE
#undef SET_DIGITAL_PIN_VALUE
#endif
#define SET_DIGITAL_PIN_VALUE   0xF5 // set value of an individual digital pin

//

#ifdef REPORT_VERSION
#undef REPORT_VERSION
#endif
#define REPORT_VERSION          0xF9 // report protocol version

#ifdef SYSTEM_RESET
#undef SYSTEM_RESET
#endif
#define SYSTEM_RESET            0xFF // reset from MIDI

//

#ifdef START_SYSEX
#undef START_SYSEX
#endif
#define START_SYSEX             0xF0 // start a MIDI Sysex message

#ifdef END_SYSEX
#undef END_SYSEX
#endif
#define END_SYSEX               0xF7 // end a MIDI Sysex message

// extended command set using sysex (0-127/0x00-0x7F)
/* 0x00-0x0F reserved for user-defined commands */

#ifdef SERIAL_MESSAGE
#undef SERIAL_MESSAGE
#endif
#define SERIAL_MESSAGE          0x60 // communicate with serial devices, including other boards

#ifdef ENCODER_DATA
#undef ENCODER_DATA
#endif
#define ENCODER_DATA            0x61 // reply with encoders current positions

#ifdef SERVO_CONFIG
#undef SERVO_CONFIG
#endif
#define SERVO_CONFIG            0x70 // set max angle, minPulse, maxPulse, freq

#ifdef STRING_DATA
#undef STRING_DATA
#endif
#define STRING_DATA             0x71 // a string message with 14-bits per char

#ifdef STEPPER_DATA
#undef STEPPER_DATA
#endif
#define STEPPER_DATA            0x72 // control a stepper motor

#ifdef ONEWIRE_DATA
#undef ONEWIRE_DATA
#endif
#define ONEWIRE_DATA            0x73 // send an OneWire read/write/reset/select/skip/search request

#ifdef SHIFT_DATA
#undef SHIFT_DATA
#endif
#define SHIFT_DATA              0x75 // a bitstream to/from a shift register

#ifdef I2C_REQUEST
#undef I2C_REQUEST
#endif
#define I2C_REQUEST             0x76 // send an I2C read/write request

#ifdef I2C_REPLY
#undef I2C_REPLY
#endif
#define I2C_REPLY               0x77 // a reply to an I2C read request

#ifdef I2C_CONFIG
#undef I2C_CONFIG
#endif
#define I2C_CONFIG              0x78 // config I2C settings such as delay times and power pins

#ifdef REPORT_FIRMWARE
#undef REPORT_FIRMWARE
#endif
#define REPORT_FIRMWARE         0x79 // report name and version of the firmware

#ifdef EXTENDED_ANALOG
#undef EXTENDED_ANALOG
#endif
#define EXTENDED_ANALOG         0x6F // analog write (PWM, Servo, etc) to any pin

#ifdef PIN_STATE_QUERY
#undef PIN_STATE_QUERY
#endif
#define PIN_STATE_QUERY         0x6D // ask for a pin's current mode and value

#ifdef PIN_STATE_RESPONSE
#undef PIN_STATE_RESPONSE
#endif
#define PIN_STATE_RESPONSE      0x6E // reply with pin's current mode and value

#ifdef CAPABILITY_QUERY
#undef CAPABILITY_QUERY
#endif
#define CAPABILITY_QUERY        0x6B // ask for supported modes and resolution of all pins

#ifdef CAPABILITY_RESPONSE
#undef CAPABILITY_RESPONSE
#endif
#define CAPABILITY_RESPONSE     0x6C // reply with supported modes and resolution

#ifdef ANALOG_MAPPING_QUERY
#undef ANALOG_MAPPING_QUERY
#endif
#define ANALOG_MAPPING_QUERY    0x69 // ask for mapping of analog to pin numbers

#ifdef ANALOG_MAPPING_RESPONSE
#undef ANALOG_MAPPING_RESPONSE
#endif
#define ANALOG_MAPPING_RESPONSE 0x6A // reply with mapping info

#ifdef SAMPLING_INTERVAL
#undef SAMPLING_INTERVAL
#endif
#define SAMPLING_INTERVAL       0x7A // set the poll rate of the main loop

#ifdef SCHEDULER_DATA
#undef SCHEDULER_DATA
#endif
#define SCHEDULER_DATA          0x7B // send a createtask/deletetask/addtotask/schedule/querytasks/querytask request to the scheduler

#ifdef SYSEX_NON_REALTIME
#undef SYSEX_NON_REALTIME
#endif
#define SYSEX_NON_REALTIME      0x7E // MIDI Reserved for non-realtime messages

#ifdef SYSEX_REALTIME
#undef SYSEX_REALTIME
#endif
#define SYSEX_REALTIME          0x7F // MIDI Reserved for realtime messages

// pin modes

#ifdef PIN_MODE_INPUT
#undef PIN_MODE_INPUT
#endif
#define PIN_MODE_INPUT          0x00 // same as INPUT defined in Arduino.h

#ifdef PIN_MODE_OUTPUT
#undef PIN_MODE_OUTPUT
#endif
#define PIN_MODE_OUTPUT         0x01 // same as OUTPUT defined in Arduino.h

#ifdef PIN_MODE_ANALOG
#undef PIN_MODE_ANALOG
#endif
#define PIN_MODE_ANALOG         0x02 // analog pin in analogInput mode

#ifdef PIN_MODE_PWM
#undef PIN_MODE_PWM
#endif
#define PIN_MODE_PWM            0x03 // digital pin in PWM output mode

#ifdef PIN_MODE_SERVO
#undef PIN_MODE_SERVO
#endif
#define PIN_MODE_SERVO          0x04 // digital pin in Servo output mode

#ifdef PIN_MODE_SHIFT
#undef PIN_MODE_SHIFT
#endif
#define PIN_MODE_SHIFT          0x05 // shiftIn/shiftOut mode

#ifdef PIN_MODE_I2C
#undef PIN_MODE_I2C
#endif
#define PIN_MODE_I2C            0x06 // pin included in I2C setup

#ifdef PIN_MODE_ONEWIRE
#undef PIN_MODE_ONEWIRE
#endif
#define PIN_MODE_ONEWIRE        0x07 // pin configured for 1-wire

#ifdef PIN_MODE_STEPPER
#undef PIN_MODE_STEPPER
#endif
#define PIN_MODE_STEPPER        0x08 // pin configured for stepper motor

#ifdef PIN_MODE_ENCODER
#undef PIN_MODE_ENCODER
#endif
#define PIN_MODE_ENCODER        0x09 // pin configured for rotary encoders

#ifdef PIN_MODE_SERIAL
#undef PIN_MODE_SERIAL
#endif
#define PIN_MODE_SERIAL         0x0A // pin configured for serial communication

#ifdef PIN_MODE_PULLUP
#undef PIN_MODE_PULLUP
#endif
#define PIN_MODE_PULLUP         0x0B // enable internal pull-up resistor for pin

#ifdef PIN_MODE_IGNORE
#undef PIN_MODE_IGNORE
#endif
#define PIN_MODE_IGNORE         0x7F // pin configured to be ignored by digitalWrite and capabilityResponse

#ifdef TOTAL_PIN_MODES
#undef TOTAL_PIN_MODES
#endif
#define TOTAL_PIN_MODES         13

#endif // FirmataConstants_h
