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

#include "FirmataConfig.h"
#include <Firmata.h>
#include "AnalogFirmata.h"

// -----------------------------------------------------------------------------
/* sets bits in a bit array (int) to toggle the reporting of the analogIns
 */
//void FirmataClass::setAnalogPinReporting(byte pin, byte state) {
//}
void reportAnalogCallback(byte analogPin, int value)
{
  AnalogFirmata.reportAnalog(analogPin,value);
}

AnalogFirmataClass::AnalogFirmataClass()
{
  analogInputsToReport = 0;
}

// -----------------------------------------------------------------------------
/* sets bits in a bit array (int) to toggle the reporting of the analogIns
 */
//void FirmataClass::setAnalogPinReporting(byte pin, byte state) {
//}
void AnalogFirmataClass::reportAnalog(byte analogPin, int value)
{
  if (analogPin < TOTAL_ANALOG_PINS) {
    if(value == 0) {
      analogInputsToReport = analogInputsToReport &~ (1 << analogPin);
    } else {
      analogInputsToReport = analogInputsToReport | (1 << analogPin);
    }
  }
  // TODO: save status to EEPROM here, if changed
}

boolean AnalogFirmataClass::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_ANALOG(pin)) {
    reportAnalog(PIN_TO_ANALOG(pin), mode == ANALOG ? 1 : 0); // turn on/off reporting
  }
  switch(mode) {
  case ANALOG:
    if (IS_PIN_ANALOG(pin)) {
      if (IS_PIN_DIGITAL(pin)) {
        pinMode(PIN_TO_DIGITAL(pin), INPUT); // disable output driver
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
      }
    }
    return true;
  case PWM:
    if (IS_PIN_PWM(pin)) {
      pinMode(PIN_TO_PWM(pin), OUTPUT);
      analogWrite(PIN_TO_PWM(pin), 0);
    }
    return true;
  }
  return false;
}

void AnalogFirmataClass::handleCapability(byte pin)
{
  if (IS_PIN_ANALOG(pin)) {
    Firmata.write(ANALOG);
    Firmata.write(10);
  }
  if (IS_PIN_PWM(pin)) {
    Firmata.write(PWM);
    Firmata.write(8);
  }
}

boolean AnalogFirmataClass::handleSysex(byte command, byte argc, byte* argv)
{
  switch(command) {
  case EXTENDED_ANALOG:
    handleExtendedAnalog(argc,argv);
    return true;
  case ANALOG_MAPPING_QUERY:
    handleAnalogMappingQuery();
    return true;
  }
  return false;
}

void AnalogFirmataClass::handleExtendedAnalog(byte argc, byte *argv)
{
  if (argc > 1) {
    int val = argv[1];
    if (argc > 2) val |= (argv[2] << 7);
    if (argc > 3) val |= (argv[3] << 14);
    analogWrite(argv[0], val);
  }
}

void AnalogFirmataClass::handleAnalogMappingQuery()
{
  Firmata.write(START_SYSEX);
  Firmata.write(ANALOG_MAPPING_RESPONSE);
  for (byte pin=0; pin < TOTAL_PINS; pin++) {
    Firmata.write(IS_PIN_ANALOG(pin) ? PIN_TO_ANALOG(pin) : 127);
  }
  Firmata.write(END_SYSEX);
}

void AnalogFirmataClass::reset()
{
  // by default, do not report any analog inputs
  analogInputsToReport = 0;
}

void AnalogFirmataClass::report()
{
  byte pin,analogPin;
  /* ANALOGREAD - do all analogReads() at the configured sampling interval */
  for(pin=0; pin<TOTAL_PINS; pin++) {
    if (IS_PIN_ANALOG(pin) && Firmata.getPinMode(pin) == ANALOG) {
      analogPin = PIN_TO_ANALOG(pin);
      if (analogInputsToReport & (1 << analogPin)) {
        Firmata.sendAnalog(analogPin, analogRead(analogPin));
      }
    }
  }
}

AnalogFirmataClass AnalogFirmata;
