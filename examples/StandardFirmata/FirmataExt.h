/*
  FirmataExt.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef FirmataExt_h
#define FirmataExt_h

#include "FirmataConfig.h"

extern "C" {
// callback function types
    typedef void (*capabilityCallbackFunction)(byte);
}

class FirmataExtClass
{
public:
/* access pin config */
  byte getPinConfig(byte pin);
  void setPinConfig(byte pin, byte config);
/* access pin state */
  int getPinState(byte pin);
  void setPinState(byte pin, int state);

  boolean handleSysex(byte command, byte argc, byte* argv);

  void attach(capabilityCallbackFunction callback);

private:
/* pins configuration */
  byte pinConfig[TOTAL_PINS];         // configuration of every pin
  int pinState[TOTAL_PINS];           // any value that has been written
  capabilityCallbackFunction capabilityFunction;
};

extern FirmataExtClass FirmataExt;
#endif
