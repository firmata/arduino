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

#include "FirmataConfig.h"
#include <Firmata.h>
#include "FirmataExt.h"

/* access pin config */
byte FirmataExtClass::getPinConfig(byte pin)
{
  return pinConfig[pin];
}

void FirmataExtClass::setPinConfig(byte pin, byte config)
{
  pinConfig[pin] = config;
}

/* access pin state */
int FirmataExtClass::getPinState(byte pin)
{
  return pinState[pin];
}

void FirmataExtClass::setPinState(byte pin, int state)
{
  pinState[pin] = state;
}

boolean FirmataExtClass::handleSysex(byte command, byte argc, byte* argv)
{
  switch (command) {

  case PIN_STATE_QUERY:
    if (argc > 0) {
      byte pin=argv[0];
      Firmata.write(START_SYSEX);
      Firmata.write(PIN_STATE_RESPONSE);
      Firmata.write(pin);
      if (pin < TOTAL_PINS) {
        Firmata.write(pinConfig[pin]);
        Firmata.write((byte)pinState[pin] & 0x7F);
        if (pinState[pin] & 0xFF80) Firmata.write((byte)(pinState[pin] >> 7) & 0x7F);
        if (pinState[pin] & 0xC000) Firmata.write((byte)(pinState[pin] >> 14) & 0x7F);
      }
      Firmata.write(END_SYSEX);
    }
    return true;
  case CAPABILITY_QUERY:
    Firmata.write(START_SYSEX);
    Firmata.write(CAPABILITY_RESPONSE);
    if (capabilityFunction) {
      for (byte pin=0; pin < TOTAL_PINS; pin++) {
          capabilityFunction(pin);
      }
      Firmata.write(127);
    }
    Firmata.write(END_SYSEX);
    break;
  }
}

void FirmataExtClass::attach(capabilityCallbackFunction function)
{
  capabilityFunction = function;
}

FirmataExtClass FirmataExt;

