/*
  DigitalOutputFirmata.cpp - Firmata library
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
#include "DigitalOutputFirmata.h"

DigitalOutputFirmata *DigitalOutputFirmataInstance;

void digitalOutputWriteCallback(byte port, int value)
{
  DigitalOutputFirmataInstance->digitalWrite(port,value);
}

DigitalOutputFirmata::DigitalOutputFirmata()
{
  DigitalOutputFirmataInstance = this;
  Firmata.attach(DIGITAL_MESSAGE, digitalOutputWriteCallback);
}

boolean DigitalOutputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  return false;
}

void DigitalOutputFirmata::reset()
{

}

void DigitalOutputFirmata::digitalWrite(byte port, int value)
{
  byte pin, lastPin, mask=1, pinWriteMask=0;

  if (port < TOTAL_PORTS) {
    // create a mask of the pins on this port that are writable.
    lastPin = port*8+8;
    if (lastPin > TOTAL_PINS) lastPin = TOTAL_PINS;
    for (pin=port*8; pin < lastPin; pin++) {
      // do not disturb non-digital pins (eg, Rx & Tx)
      if (IS_PIN_DIGITAL(pin)) {
        // only write to OUTPUT and INPUT (enables pullup)
        // do not touch pins in PWM, ANALOG, SERVO or other modes
        byte pinMode = Firmata.getPinMode(pin);
        if (pinMode == OUTPUT || pinMode == INPUT) {
          pinWriteMask |= mask;
          Firmata.setPinState(pin,((byte)value & mask) ? 1 : 0);
        }
      }
      mask = mask << 1;
    }
    writePort(port, (byte)value, pinWriteMask);
  }
}

boolean DigitalOutputFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_DIGITAL(pin) && mode == OUTPUT && Firmata.getPinMode(pin) != IGNORE) {
    digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable PWM
    pinMode(PIN_TO_DIGITAL(pin), OUTPUT);
    return true;
  }
  return false;
}

void DigitalOutputFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write((byte)OUTPUT);
    Firmata.write(1);
  }
}
