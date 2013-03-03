/*
  AnalogFirmata.h - Firmata library
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
#include <AnalogFirmata.h>
#include <AnalogOutputFirmata.h>

//AnalogOutputFirmataClass::AnalogOutputFirmataClass()
//{
//  Firmata.attach(REPORT_ANALOG, analogWriteCallback); //TODO: analogWriteCallback is the same for PWM and SERVO
//}

boolean AnalogOutputFirmataClass::handlePinMode(byte pin, int mode)
{
  if (mode == PWM && IS_PIN_PWM(pin)) {
    pinMode(PIN_TO_PWM(pin), OUTPUT);
    analogWrite(PIN_TO_PWM(pin), 0);
    return true;
  }
  return false;
}

void AnalogOutputFirmataClass::handleCapability(byte pin)
{
  if (IS_PIN_PWM(pin)) {
    Firmata.write(PWM);
    Firmata.write(8);
  }
}

boolean AnalogOutputFirmataClass::handleSysex(byte command, byte argc, byte* argv)
{
  return AnalogFirmata.handleSysex(command, argc, argv);
}

AnalogOutputFirmataClass AnalogOutputFirmata;
