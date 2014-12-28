/*
  FirmataReporting.cpp - Firmata library
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
#include "FirmataFeature.h"
#include "FirmataReporting.h"

void FirmataReporting::setSamplingInterval(int interval)
{
  samplingInterval = interval;
}

void FirmataReporting::handleCapability(byte pin)
{

}

boolean FirmataReporting::handlePinMode(byte pin, int mode)
{
  return false;
}

boolean FirmataReporting::handleSysex(byte command, byte argc, byte* argv)
{
  if (command == SAMPLING_INTERVAL) {
    if (argc > 1) {
      samplingInterval = argv[0] + (argv[1] << 7);
      if (samplingInterval < MINIMUM_SAMPLING_INTERVAL) {
        samplingInterval = MINIMUM_SAMPLING_INTERVAL;
      }
      return true;
    }
  }
  return false;
}

boolean FirmataReporting::elapsed()
{
  currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    previousMillis += samplingInterval;
    if (currentMillis - previousMillis > samplingInterval)
      previousMillis = currentMillis-samplingInterval;
    return true;
  }
  return false;
}

void FirmataReporting::reset()
{
  previousMillis = millis();
  samplingInterval = 19;
}
