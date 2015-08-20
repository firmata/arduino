/*
  Firmata.h - Firmata library v2.4.4 - 2015-8-9
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef Firmata_h
#define Firmata_h

#include "Boards.h"  /* Hardware Abstraction Layer + Wiring/Arduino */
#include "Firmata_Constants.h" /* Constants used by the protocol */

extern "C" {
  // callback function types
  typedef void (*callbackFunction)(byte, int);
  typedef void (*systemResetCallbackFunction)(void);
  typedef void (*stringCallbackFunction)(char *);
  typedef void (*sysexCallbackFunction)(byte command, byte argc, byte *argv);
}


// TODO make it a subclass of a generic Serial/Stream base class
class FirmataClass
{
  public:
    FirmataClass();
    /* Arduino constructors */
    void begin();
    void begin(long);
    void begin(Stream &s);
    /* querying functions */
    void printVersion(void);
    void blinkVersion(void);
    void printFirmwareVersion(void);
    //void setFirmwareVersion(byte major, byte minor);  // see macro below
    void setFirmwareNameAndVersion(const char *name, byte major, byte minor);
    /* serial receive handling */
    int available(void);
    void processInput(void);
    /* serial send handling */
    void sendAnalog(byte pin, int value);
    void sendDigital(byte pin, int value); // TODO implement this
    void sendDigitalPort(byte portNumber, int portData);
    void sendString(const char *string);
    void sendString(byte command, const char *string);
    void sendSysex(byte command, byte bytec, byte *bytev);
    void write(byte c);
    /* attach & detach callback functions to messages */
    void attach(byte command, callbackFunction newFunction);
    void attach(byte command, systemResetCallbackFunction newFunction);
    void attach(byte command, stringCallbackFunction newFunction);
    void attach(byte command, sysexCallbackFunction newFunction);
    void detach(byte command);

    /* utility methods */
    void sendValueAsTwo7bitBytes(int value);
    void startSysex(void);
    void endSysex(void);

  private:
    Stream *FirmataStream;
    /* firmware name and version */
    byte firmwareVersionCount;
    byte *firmwareVersionVector;
    /* input message handling */
    byte waitForData; // this flag says the next serial input will be data
    byte executeMultiByteCommand; // execute this after getting multi-byte data
    byte multiByteChannel; // channel data for multiByteCommands
    byte storedInputData[MAX_DATA_BYTES]; // multi-byte data
    /* sysex */
    boolean parsingSysex;
    int sysexBytesRead;
    /* callback functions */
    callbackFunction currentAnalogCallback;
    callbackFunction currentDigitalCallback;
    callbackFunction currentReportAnalogCallback;
    callbackFunction currentReportDigitalCallback;
    callbackFunction currentPinModeCallback;
    systemResetCallbackFunction currentSystemResetCallback;
    stringCallbackFunction currentStringCallback;
    sysexCallbackFunction currentSysexCallback;

    /* private methods ------------------------------ */
    void processSysexMessage(void);
    void systemReset(void);
    void strobeBlinkPin(int count, int onInterval, int offInterval);
};

extern FirmataClass Firmata;

/*==============================================================================
 * MACROS
 *============================================================================*/

/* shortcut for setFirmwareNameAndVersion() that uses __FILE__ to set the
 * firmware name.  It needs to be a macro so that __FILE__ is included in the
 * firmware source file rather than the library source file.
 */
#define setFirmwareVersion(x, y)   setFirmwareNameAndVersion(__FILE__, x, y)

#endif /* Firmata_h */
