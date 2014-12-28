/*
  ServoFirmata.h - Firmata library
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

#ifndef ServoFirmata_h
#define ServoFirmata_h

#include <Servo.h>
#include <Firmata.h>
#include "FirmataFeature.h"

void servoAnalogWrite(byte pin, int value);

class ServoFirmata:public FirmataFeature
{
public:
  ServoFirmata();
  boolean analogWrite(byte pin, int value);
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();
private:
  Servo *servos[MAX_SERVOS];
  void attach(byte pin, int minPulse, int maxPulse);
  void detach(byte pin);
};

#endif
