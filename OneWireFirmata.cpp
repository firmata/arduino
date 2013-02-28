/*
  OneWireFirmata.cpp - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include <Firmata.h>
#include <OneWireFirmata.h>
#include <Encoder7Bit.h>
#ifdef FIRMATASCHEDULER
#include <FirmataScheduler.h>
#endif

boolean OneWireFirmataClass::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_DIGITAL(pin) && mode == ONEWIRE) {
    OneWireFirmata.oneWireConfig(pin,ONEWIRE_POWER);
    return true;
  }
  return false;
}

void OneWireFirmataClass::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write((byte)ONEWIRE);
    Firmata.write(1);
  }
}

void OneWireFirmataClass::oneWireConfig(byte pin, boolean power)
{
  ow_device_info *info = &pinOneWire[pin];
  if (info->device==NULL) {
    info->device = new OneWire(pin);
  }
  for (int i=0;i<8;i++) {
    info->addr[i]=0x0;
  }
  info->power = power;
}

boolean OneWireFirmataClass::handleSysex(byte command, byte argc, byte* argv)
{
  if (command == ONEWIRE_DATA) {
    if (argc>1) {
      byte subcommand = argv[0];
      byte pin = argv[1];
      ow_device_info *info = &pinOneWire[pin];
      OneWire *device = info->device;
      if (device) {
        switch(subcommand) {
        case ONEWIRE_SEARCH_REQUEST:
        case ONEWIRE_SEARCH_ALARMS_REQUEST:
          {
            device->reset_search();
            Firmata.write(START_SYSEX);
            Firmata.write(ONEWIRE_DATA);
            boolean isAlarmSearch = (subcommand == ONEWIRE_SEARCH_ALARMS_REQUEST);
            Firmata.write(isAlarmSearch ? (byte)ONEWIRE_SEARCH_ALARMS_REPLY : (byte)ONEWIRE_SEARCH_REPLY);
            Firmata.write(pin);
            Encoder7Bit.startBinaryWrite();
            byte addrArray[8];
            while (isAlarmSearch ? device->search_alarms(addrArray) : device->search(addrArray)) {
              for (int i=0;i<8;i++) {
                Encoder7Bit.writeBinary(addrArray[i]);
              }
            }
            Encoder7Bit.endBinaryWrite();
            Firmata.write(END_SYSEX);
            break;
          }
        case ONEWIRE_CONFIG_REQUEST:
          {
            if (argc==3) {
              Firmata.setPinMode(pin,ONEWIRE);
              oneWireConfig(pin, argv[2]); // this calls oneWireConfig again, this time setting the correct config (which doesn't cause harm though)
            }
            break;
          }
        default:
          {
            if (subcommand & ONEWIRE_RESET_REQUEST_BIT) {
              device->reset();
              for (int i=0;i<8;i++) {
                info->addr[i]=0x0;
              }
            }
            if (subcommand & ONEWIRE_SKIP_REQUEST_BIT) {
              device->skip();
              for (byte i=0;i<8;i++) {
                info->addr[i]=0x0;
              }
            }
            if (subcommand & ONEWIRE_WITHDATA_REQUEST_BITS) {
              int numBytes=num7BitOutbytes(argc-2);
              int numReadBytes=0;
              argv+=2;
              Encoder7Bit.readBinary(numBytes,argv,argv); //decode inplace

              if (subcommand & ONEWIRE_SELECT_REQUEST_BIT) {
                if (numBytes<8) break;
                device->select(argv);
                for (int i=0;i<8;i++) {
                  info->addr[i]=argv[i];
                }
                argv+=8;
                numBytes-=8;
              }

              if (subcommand & ONEWIRE_READ_REQUEST_BIT) {
                if (numBytes<2) break;
                numReadBytes = *((int*)argv);
                argv+=2;
                numBytes-=2;
              }

              if (subcommand & ONEWIRE_DELAY_REQUEST_BIT) {
                if (numBytes<4) break;
                Firmata.delayTask(*((long*)argv));
                argv+=4;
                numBytes-=4;
              }

              if (subcommand & ONEWIRE_WRITE_REQUEST_BIT) {
                for (int i=0;i<numBytes;i++) {
                  info->device->write(argv[i],info->power);
                }
              }

              if (numReadBytes>0) {
                Firmata.write(START_SYSEX);
                Firmata.write(ONEWIRE_DATA);
                Firmata.write(ONEWIRE_READ_REPLY);
                Firmata.write(pin);
                Encoder7Bit.startBinaryWrite();
                for (int i=0;i<8;i++) {
                  Encoder7Bit.writeBinary(info->addr[i]);
                }
                for (int i=0;i<numReadBytes;i++) {
                  Encoder7Bit.writeBinary(device->read());
                }
                Encoder7Bit.endBinaryWrite();
                Firmata.write(END_SYSEX);
              }
            }
          }
        }
      }
    }
    return true;
  }
  return false;
}

void OneWireFirmataClass::reset()
{
  for (int i=0;i<TOTAL_PINS;i++) {
    if (pinOneWire[i].device) {
      free(pinOneWire[i].device);
      pinOneWire[i].device=NULL;
    }
    for (int j=0;j<8;j++) {
      pinOneWire[i].addr[j]=0;
    }
    pinOneWire[i].power=false;
  }
}

OneWireFirmataClass OneWireFirmata;

