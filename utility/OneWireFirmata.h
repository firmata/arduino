/*
  OneWireFirmata.h - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef OneWireFirmata_h
#define OneWireFirmata_h

#include "OneWire.h"
#include <Firmata.h>
#include "FirmataFeature.h"

//subcommands:
#define ONEWIRE_SEARCH_REQUEST 0x40
#define ONEWIRE_CONFIG_REQUEST 0x41
#define ONEWIRE_SEARCH_REPLY 0x42
#define ONEWIRE_READ_REPLY 0x43
#define ONEWIRE_SEARCH_ALARMS_REQUEST 0x44
#define ONEWIRE_SEARCH_ALARMS_REPLY 0x45

#define ONEWIRE_RESET_REQUEST_BIT 0x01
#define ONEWIRE_SKIP_REQUEST_BIT 0x02
#define ONEWIRE_SELECT_REQUEST_BIT 0x04
#define ONEWIRE_READ_REQUEST_BIT 0x08
#define ONEWIRE_DELAY_REQUEST_BIT 0x10
#define ONEWIRE_WRITE_REQUEST_BIT 0x20

#define ONEWIRE_WITHDATA_REQUEST_BITS 0x3C

#define ONEWIRE_CRC 0 //for OneWire.h: crc-functions are not used by Firmata

//default value for power:
#define ONEWIRE_POWER 1

struct ow_device_info
{
  OneWire* device;
  byte addr[8];
  boolean power;
};

class OneWireFirmata:public FirmataFeature
{
public:
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();
  
private:
  ow_device_info pinOneWire[TOTAL_PINS];
  void oneWireConfig(byte pin, boolean power);
};

#endif


