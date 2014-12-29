/*
  ServoFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2014 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include <Servo.h>
#include <Firmata.h>
#include <ServoFirmata.h>

ServoFirmata *ServoInstance;

void servoAnalogWrite(byte pin, int value)
{
  ServoInstance->analogWrite(pin, value);
}

ServoFirmata::ServoFirmata()
{
  ServoInstance = this;
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    servoPinMap[pin] = 255;
  }  
  detachedServoCount = 0;
  servoCount = 0;
}

boolean ServoFirmata::analogWrite(byte pin, int value)
{
  if (IS_PIN_DIGITAL(pin) && servoPinMap[pin] != 255) {
    servos[servoPinMap[pin]].write(value);
  }
}

boolean ServoFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == SERVO) {
      Firmata.setPinConfig(pin, mode);
      attach(pin, -1, -1);
      return true;
    } else {
      if (servoPinMap[pin] < MAX_SERVOS && servos[servoPinMap[pin]].attached()) {
        detach(pin);
      }
    }
  }
  return false;
}

void ServoFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
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
      if (IS_PIN_DIGITAL(pin) && Firmata.getPinMode(pin)!=IGNORE) {
        int minPulse = argv[1] + (argv[2] << 7);
        int maxPulse = argv[3] + (argv[4] << 7);
        attach(pin, minPulse, maxPulse);
        Firmata.setPinMode(pin, SERVO);
        return true;
      }
    }
  }
  return false;
}

void ServoFirmata::attach(byte pin, int minPulse, int maxPulse)
{
  if (servoCount >= MAX_SERVOS) {
    Firmata.sendString("Max servos attached");
    return;
  }
 
  if (servoPinMap[pin] != 255 && servos[servoPinMap[pin]].attached())
    servos[servoPinMap[pin]].detach();

  // reuse indexes of detached servos until all have been reallocated
  if (detachedServoCount > 0) {
    servoPinMap[pin] = detachedServos[detachedServoCount - 1];
    if (detachedServoCount > 0) detachedServoCount--;
  } else {
    servoPinMap[pin] = servoCount;
    servoCount++;
  }
  if (minPulse >= 0 || maxPulse >= 0)
    servos[servoPinMap[pin]].attach(PIN_TO_DIGITAL(pin), minPulse, maxPulse);
  else
    servos[servoPinMap[pin]].attach(PIN_TO_DIGITAL(pin));
}

void ServoFirmata::detach(byte pin)
{
  if (servoPinMap[pin] == 255)
    return;
  
  if (servos[servoPinMap[pin]].attached()) {
    servos[servoPinMap[pin]].detach();

    // if we're detaching the last servo, decrement the count
    // otherwise store the index of the detached servo
    if (servoPinMap[pin] == servoCount && servoCount > 0) {
      servoCount--;
    } else if (servoCount > 0) {
      // keep track of detached servos because we want to reuse their indexes
      // before incrementing the count of attached servos
      detachedServoCount++;
      detachedServos[detachedServoCount - 1] = servoPinMap[pin];
    }
  }

  servoPinMap[pin] = 255;
}

void ServoFirmata::reset()
{
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_DIGITAL(pin)) {
      detach(pin);
    }
  }
  detachedServoCount = 0;
  servoCount = 0;
}
