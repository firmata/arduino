/*
  DigitalOutputFirmata.h - Firmata library
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

#ifndef DigitalOutputFirmata_h
#define DigitalOutputFirmata_h

#include <Firmata.h>
#include "FirmataFeature.h"

void digitalOutputWriteCallback(byte port, int value);

class DigitalOutputFirmata:public FirmataFeature
{
public:
  DigitalOutputFirmata();
  void digitalWrite(byte port, int value);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  boolean handlePinMode(byte pin, int mode);
  void reset();
private:
};

#endif
