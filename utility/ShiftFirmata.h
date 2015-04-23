/*
  ShiftFirmata.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2013 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef ShiftFirmata_h
#define ShiftFirmata_h

#include <Firmata.h>
#include <utility/FirmataFeature.h>

// shift commands
#define SHIFT_CONFIG 1
#define SHIFT_OUT 2
#define SHIFT_IN 3
#define SHIFT_IN_REPLY 4

// shift types
// SHIFT_OUT 2
// SHIFT_IN 3
#define LATCH_L_SHIFT_OUT 4
#define SHIFT_OUT_LATCH_H 5
#define LATCH_L_SHIFT_OUT_LATCH_H 6
#define TOGGLE_LOAD_SHIFT_IN 7

#define SHIFT_COMMAND_MASK  B00000111
#define SHIFT_TYPE_MASK  B00111000

struct shift_info
{
  byte type;
  byte dataPin;
  byte clockPin;
  byte bitOrder;
  byte latchPin; // optional
};

class ShiftFirmata:public FirmataFeature
{
public:
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();

private:
  byte shift_In(byte dataPin, byte clockPin, byte bitOrder);
  shift_info pinShift[TOTAL_PINS];
};

#endif
