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

#include "FirmataConstants.h"

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
