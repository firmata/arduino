/*
  FirmataExt.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include <Firmata.h>
#include <FirmataExt.h>

void setPinModeCallback(byte pin, int mode)
{
  FirmataExt.handlePinMode(pin,mode);
}

void handleSysexCallback(byte command, byte argc, byte* argv)
{
  FirmataExt.handleSysex(command, argc, argv);
}

FirmataExtClass::FirmataExtClass()
{
  Firmata.attach(SET_PIN_MODE, setPinModeCallback);
  Firmata.attach((byte)START_SYSEX,handleSysexCallback);
  numCapabilities = 0;
}

void FirmataExtClass::handlePinMode(byte pin, byte mode)
{
  boolean known = false;
  for (byte i=0;i<numCapabilities;i++) {
    known |= capabilities[i]->handlePinMode(pin,mode);
  }
  if (!known && mode != IGNORE) {
    Firmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
  }
}

void FirmataExtClass::handleSysex(byte command, byte argc, byte* argv)
{
  switch (command) {

  case PIN_STATE_QUERY:
    if (argc > 0) {
      byte pin=argv[0];
      if (pin < TOTAL_PINS) {
        Firmata.write(START_SYSEX);
        Firmata.write(PIN_STATE_RESPONSE);
        Firmata.write(pin);
        Firmata.write(Firmata.getPinMode(pin));
        int pinState = Firmata.getPinState(pin);
        Firmata.write((byte)pinState & 0x7F);
        if (pinState & 0xFF80) Firmata.write((byte)(pinState >> 7) & 0x7F);
        if (pinState & 0xC000) Firmata.write((byte)(pinState >> 14) & 0x7F);
        Firmata.write(END_SYSEX);
      }
    }
    return;
  case CAPABILITY_QUERY:
    Firmata.write(START_SYSEX);
    Firmata.write(CAPABILITY_RESPONSE);
    for (byte pin=0; pin < TOTAL_PINS; pin++) {
      if (Firmata.getPinMode(pin) != IGNORE) {
        for (byte i=0;i<numCapabilities;i++) {
          capabilities[i]->handleCapability(pin);
        }
      }
      Firmata.write(127);
    }
    Firmata.write(END_SYSEX);
    return;
  default:
    for (byte i=0;i<numCapabilities;i++) {
      if (capabilities[i]->handleSysex(command,argc,argv)) {
          return;
      }
    }
    Firmata.sendString("Unhandled sysex command");
    break;
  }
}

void FirmataExtClass::addFeature(FirmataFeature &capability)
{
  if (numCapabilities<MAX_FEATURES) {
    capabilities[numCapabilities++] = &capability;
  }
}

void FirmataExtClass::reset()
{
  for (byte i=0;i<numCapabilities;i++) {
    capabilities[i]->reset();
  }
}

FirmataExtClass FirmataExt;

