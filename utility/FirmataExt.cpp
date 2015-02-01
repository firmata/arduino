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
#include "FirmataExt.h"

FirmataExt *FirmataExtInstance;

void handleSetPinModeCallback(byte pin, int mode)
{
  if (!FirmataExtInstance->handlePinMode(pin,mode) && mode != IGNORE) {
    Firmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
  }
}

void handleSysexCallback(byte command, byte argc, byte* argv)
{
  if (!FirmataExtInstance->handleSysex(command, argc, argv)) {
    Firmata.sendString("Unhandled sysex command");
  }
}

FirmataExt::FirmataExt()
{
  FirmataExtInstance = this;
  Firmata.attach(SET_PIN_MODE, handleSetPinModeCallback);
  Firmata.attach((byte)START_SYSEX,handleSysexCallback);
  numFeatures = 0;
}

void FirmataExt::handleCapability(byte pin)
{

}

boolean FirmataExt::handlePinMode(byte pin, int mode)
{
  boolean known = false;
  for (byte i=0;i<numFeatures;i++) {
    known |= features[i]->handlePinMode(pin,mode);
  }
  return known;
}

boolean FirmataExt::handleSysex(byte command, byte argc, byte* argv)
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
        return true;
      }
    }
    break;
  case CAPABILITY_QUERY:
    Firmata.write(START_SYSEX);
    Firmata.write(CAPABILITY_RESPONSE);
    for (byte pin=0; pin < TOTAL_PINS; pin++) {
      if (Firmata.getPinMode(pin) != IGNORE) {
        for (byte i=0;i<numFeatures;i++) {
          features[i]->handleCapability(pin);
        }
      }
      Firmata.write(127);
    }
    Firmata.write(END_SYSEX);
    return true;
  default:
    for (byte i=0;i<numFeatures;i++) {
      if (features[i]->handleSysex(command,argc,argv)) {
        return true;
      }
    }
    break;
  }
  return false;
}

void FirmataExt::addFeature(FirmataFeature &capability)
{
  if (numFeatures<MAX_FEATURES) {
    features[numFeatures++] = &capability;
  }
}

void FirmataExt::reset()
{
  for (byte i=0;i<numFeatures;i++) {
    features[i]->reset();
  }
}
