/*
  ServoFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "FirmataConfig.h"
#include <Firmata.h>
#include "ServoFirmata.h"

#ifdef FIRMATAEXT
#include "FirmataExt.h"
#endif

boolean ServoFirmataClass::analogWrite(byte pin, int value)
{
  servos[PIN_TO_SERVO(pin)].write(value);
}

boolean ServoFirmataClass::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_SERVO(pin)) {
    if (mode==SERVO) {
#ifdef FIRMATAEXT
      FirmataExt.setPinConfig(pin,SERVO);
#endif
      if (!servos[PIN_TO_SERVO(pin)].attached()) {
        servos[PIN_TO_SERVO(pin)].attach(PIN_TO_DIGITAL(pin));
      }
      return true;
    } else if (servos[PIN_TO_SERVO(pin)].attached()) {
      servos[PIN_TO_SERVO(pin)].detach();
    }
  }
  return false;
}

void ServoFirmataClass::handleCapability(byte pin)
{
  if (IS_PIN_SERVO(pin)) {
    Firmata.write(SERVO);
    Firmata.write(14);
  }
}

boolean ServoFirmataClass::handleSysex(byte command, byte argc, byte* argv)
{
  if(command == SERVO_CONFIG) {
    if (argc > 4) {
      // these vars are here for clarity, they'll optimized away by the compiler
      byte pin = argv[0];
      int minPulse = argv[1] + (argv[2] << 7);
      int maxPulse = argv[3] + (argv[4] << 7);

      if (IS_PIN_SERVO(pin)) {
        if (servos[PIN_TO_SERVO(pin)].attached())
          servos[PIN_TO_SERVO(pin)].detach();
        servos[PIN_TO_SERVO(pin)].attach(PIN_TO_DIGITAL(pin), minPulse, maxPulse);
#ifdef FIRMATAEXT
        FirmataExt.setPinConfig(pin, SERVO);
#endif
      }
    }
    return true;
  }
  return false;
}

boolean ServoFirmataClass::handlePinState(byte pin, byte pinConfig)
{
  if (pinConfig==SERVO) {
    Firmata.write((byte)pinState[pin] & 0x7F);
    if (pinState[pin] & 0xFF80) Firmata.write((byte)(pinState[pin] >> 7) & 0x7F);
    if (pinState[pin] & 0xC000) Firmata.write((byte)(pinState[pin] >> 14) & 0x7F);
    return true;
  }
  return false;
}

ServoFirmataClass ServoFirmata;
