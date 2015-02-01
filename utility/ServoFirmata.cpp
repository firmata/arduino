/*
  ServoFirmata.cpp - Firmata library
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

#include <Servo.h>
#include <Firmata.h>
#include "ServoFirmata.h"

ServoFirmata *ServoInstance;

void servoAnalogWrite(byte pin, int value)
{
  ServoInstance->analogWrite(pin,value);
}

ServoFirmata::ServoFirmata()
{
  ServoInstance = this;
}

boolean ServoFirmata::analogWrite(byte pin, int value)
{
  if (IS_PIN_SERVO(pin)) {
    Servo *servo = servos[PIN_TO_SERVO(pin)];
    if (servo)
      servo->write(value);
  }
}

boolean ServoFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_SERVO(pin)) {
    if (mode==SERVO) {
      attach(pin,-1,-1);
      return true;
    } else {
      detach(pin);
    }
  }
  return false;
}

void ServoFirmata::handleCapability(byte pin)
{
  if (IS_PIN_SERVO(pin)) {
    Firmata.write(SERVO);
    Firmata.write(14); //14 bit resolution (Servo takes int as argument)
  }
}

boolean ServoFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  if(command == SERVO_CONFIG) {
    if (argc > 4) {
      // these vars are here for clarity, they'll optimized away by the compiler
      byte pin = argv[0];
      if (IS_PIN_SERVO(pin) && Firmata.getPinMode(pin)!=IGNORE) {
        int minPulse = argv[1] + (argv[2] << 7);
        int maxPulse = argv[3] + (argv[4] << 7);
        Firmata.setPinMode(pin, SERVO);
        attach(pin, minPulse, maxPulse);
        return true;
      }
    }
  }
  return false;
}

void ServoFirmata::attach(byte pin, int minPulse, int maxPulse)
{
  Servo *servo = servos[PIN_TO_SERVO(pin)];
  if (!servo) {
    servo = new Servo();
    servos[PIN_TO_SERVO(pin)] = servo;
  }
  if (servo->attached())
    servo->detach();
  if (minPulse>=0 || maxPulse>=0)
    servo->attach(PIN_TO_DIGITAL(pin),minPulse,maxPulse);
  else
    servo->attach(PIN_TO_DIGITAL(pin));
}

void ServoFirmata::detach(byte pin)
{
  Servo *servo = servos[PIN_TO_SERVO(pin)];
  if (servo) {
    if (servo->attached())
      servo->detach();
    free(servo);
    servos[PIN_TO_SERVO(pin)]=NULL;
  }
}

void ServoFirmata::reset()
{
  for (byte pin=0;pin<TOTAL_PINS;pin++) {
    if (IS_PIN_SERVO(pin)) {
      detach(pin);
    }
  }
}
