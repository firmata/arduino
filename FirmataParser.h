/*
  FirmataParser.h - Firmata library v2.5.4 - 2016-10-23
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef FirmataParser_h
#define FirmataParser_h

#if defined(__cplusplus) && !defined(ARDUINO)
  #include <cstddef>
  #include <cstdint>
#else
  #include <stddef.h>
  #include <stdint.h>
#endif

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
#ifdef STRING_DATA
#undef STRING_DATA
#endif
#define STRING_DATA             0x71 // a string message with 14-bits per char

#ifdef REPORT_FIRMWARE
#undef REPORT_FIRMWARE
#endif
#define REPORT_FIRMWARE         0x79 // report name and version of the firmware

extern "C" {
  // callback function types
  typedef void (*callbackFunction)(uint8_t, int);
  typedef void (*systemCallbackFunction)(void);
  typedef void (*stringCallbackFunction)(char *);
  typedef void (*sysexCallbackFunction)(uint8_t command, uint8_t argc, uint8_t *argv);
}

class FirmataParser
{
  public:
    FirmataParser();
    /* serial receive handling */
    void parse(uint8_t value);
    bool isParsingMessage(void);
    /* attach & detach callback functions to messages */
    void attach(uint8_t command, callbackFunction newFunction);
    void attach(uint8_t command, systemCallbackFunction newFunction);
    void attach(uint8_t command, stringCallbackFunction newFunction);
    void attach(uint8_t command, sysexCallbackFunction newFunction);
    void detach(uint8_t command);

  private:
    /* input message handling */
    uint8_t executeMultiByteCommand; // execute this after getting multi-byte data
    uint8_t multiByteChannel; // channel data for multiByteCommands
    uint8_t storedInputData[MAX_DATA_BYTES]; // multi-byte data
    size_t waitForData; // this flag says the next serial input will be data
    /* sysex */
    bool parsingSysex;
    size_t sysexBytesRead;

    /* callback functions */
    callbackFunction currentAnalogCallback;
    callbackFunction currentDigitalCallback;
    callbackFunction currentReportAnalogCallback;
    callbackFunction currentReportDigitalCallback;
    callbackFunction currentPinModeCallback;
    callbackFunction currentPinValueCallback;
    systemCallbackFunction currentReportFirmwareCallback;
    systemCallbackFunction currentReportVersionCallback;
    systemCallbackFunction currentSystemResetCallback;
    stringCallbackFunction currentStringCallback;
    sysexCallbackFunction currentSysexCallback;

    /* private methods ------------------------------ */
    void processSysexMessage(void);
    void systemReset(void);
};

#endif /* FirmataParser_h */
