/*
  ServoFirmata.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef ServoFirmata_h
#define ServoFirmata_h

#include <Firmata.h>
#include <Servo.h>

class ServoFirmataClass
{
public:
  boolean analogWrite(byte pin, int value);
  boolean handlePinMode(byte pin, int mode);
  boolean handlePinState(byte pin, byte pinConfig);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
private:
  Servo servos[MAX_SERVOS];
  int pinState[TOTAL_PINS];           // any value that has been written
};

extern ServoFirmataClass ServoFirmata;

#endif
