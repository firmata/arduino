/*
  FirmataExt.h - Firmata library
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

#ifndef FirmataExt_h
#define FirmataExt_h

#include <Firmata.h>
#include "FirmataFeature.h"

#define MAX_FEATURES TOTAL_PIN_MODES+1

void handleSetPinModeCallback(byte pin, int mode);

void handleSysexCallback(byte command, byte argc, byte* argv);

class FirmataExt:public FirmataFeature
{
public:
  FirmataExt();
  void handleCapability(byte pin); //empty method
  boolean handlePinMode(byte pin, int mode);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void addFeature(FirmataFeature &capability);
  void reset();

private:
  FirmataFeature *features[MAX_FEATURES];
  byte numFeatures;
};

#endif
