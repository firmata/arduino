/*
  ToneFirmata.h - Firmata library
  Copyright (C) 2013 Alan Yorinks. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef Tone_Firmata_h
#define Tone_Firmata_h

#include <Firmata.h>
#include <utility/FirmataFeature.h>
#include <Arduino.h>

class ToneFirmata:public FirmataFeature
{
public:
  void toneRequest(byte pin, int frequency, int duration);
  boolean handleSysex(byte command, byte argc, byte *argv);
  void handleCapability(byte pin) ;
  boolean handlePinMode(byte pin, int mode);
  void reset();

private: // empty
};

#endif
