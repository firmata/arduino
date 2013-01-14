#ifndef OneWireFirmata_h
#define OneWireFirmata_h

#include <OneWire.h>
#include "Arduino.h"
#include "Boards.h"
#include "Firmata.h"
#include "FirmataExt.h"
#include "Encoder7Bit.h"
#include "FirmataScheduler.h"

//sysex commands:
#define ONEWIRE_REQUEST         0x73 // send an OneWire read/write/reset/select/skip/search request
#define ONEWIRE_REPLY           0x74 // reply to a OneWire read/search request

//subcommands:
#define ONEWIRE_SEARCH_REQUEST 0x40
#define ONEWIRE_CONFIG_REQUEST 0x41
#define ONEWIRE_SEARCH_REPLY 0x42
#define ONEWIRE_READ_REPLY 0x43

#define ONEWIRE_RESET_REQUEST_BIT 0x01
#define ONEWIRE_SKIP_REQUEST_BIT 0x02
#define ONEWIRE_SELECT_REQUEST_BIT 0x04
#define ONEWIRE_READ_REQUEST_BIT 0x08
#define ONEWIRE_DELAY_REQUEST_BIT 0x10
#define ONEWIRE_WRITE_REQUEST_BIT 0x20

#define ONEWIRE_WITHDATA_REQUEST_BITS 0x3C

//default value for power:
#define ONEWIRE_POWER 1

struct ow_device_info {
  OneWire* device;
  byte addr[8];
  boolean power;
};

class OneWireFirmataClass {

public:

  void handleOneWireRequest(byte command, byte argc, byte *argv);
  void oneWireConfig(byte pin, boolean power);
  void reset();
  
private:
  ow_device_info pinOneWire[TOTAL_PINS];
};

extern OneWireFirmataClass OneWireFirmata;

#endif


