/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please clink on the following link
 * to open the download page in your default browser.
 *
 * http://firmata.org/wiki/Download
 */

/*
 Copyright (C) 2009 Jeff Hoefs.  All rights reserved.
 Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 See file LICENSE.txt for further informations on licensing terms.
 */

#include <OneWire.h>
#include <Boards.h>
#include <Firmata.h>

#define ONEWIRE_REQUEST 0x60
#define ONEWIRE_REPLY 0x61
#define ONEWIRE_CONFIG 0x62

#define ONEWIRE_SEARCH 0
#define ONEWIRE_RESET 1
#define ONEWIRE_SELECT 2
#define ONEWIRE_SKIP 3
#define ONEWIRE_WRITE 4
#define ONEWIRE_READ 5

#define POWER 1

// 1-Wire Pinmode
#define ONEWIRE 0x07

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* pins configuration */
byte pinConfig[TOTAL_PINS];         // configuration of every pin
int pinState[TOTAL_PINS];           // any value that has been written. (Does this make sense for OneWire?)

struct ow_device_info {
  OneWire* device;
  byte addr[8];
  boolean power;
};

ow_device_info pinOneWire[TOTAL_PINS];

void oneWireConfig(byte pin, boolean power) {
  pinConfig[pin] = ONEWIRE;
  ow_device_info *owInfo = &pinOneWire[pin];
  if (owInfo->device==NULL) {
    owInfo->device = new OneWire(pin);
  }
  for (int i=0;i<8;i++) {
    owInfo->addr[i]=0x0;
  }
  owInfo->power = power;
}

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/

void sysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case ONEWIRE_REQUEST: 
    {
      byte pin = argv[0];
      ow_device_info *info = &pinOneWire[pin];
      byte owCommand = argv[1];
      OneWire *device = info->device;
      if (device) {
        switch(owCommand) {
        case ONEWIRE_SEARCH: 
          {
            device->reset_search();
            Serial.write(START_SYSEX);
            Serial.write(ONEWIRE_REPLY);
            Serial.write(pin);
            Serial.write((byte)ONEWIRE_SEARCH);
            byte addrArray[8];
            while (device->search(addrArray)) {
              for (int i=0;i<8;i++) {
                Serial.write(addrArray[i] & 0b01111111);
                Serial.write((byte)((addrArray[i] << 7) & 0b01110000));
              }
            }
            Serial.write(END_SYSEX);
            break;
          }
        case ONEWIRE_RESET:
          device->reset();
          for (int i=0;i<8;i++) {
            info->addr[i]=0x0;
          }
          break;
        case ONEWIRE_SELECT:
          if (argc >= 18) {
            for (int i=0;i<8;i++) {
              info->addr[i]= (argv[i<<1+2] & 0b01111111 + argv[i<<1+3] << 7) & 0b11111111;
            }
            device->select(info->addr);
          }
          break;
        case ONEWIRE_SKIP:
          for (int i=0;i<8;i++) {
            info->addr[i]=0x0;
          }
          device->skip();
          break;
        case ONEWIRE_WRITE:
          for (int i=2;i<argc;i+=2) {
            byte value = (argv[i] & 0x7F + argv[i+1] << 7) & 0xFF;
            device->write(value,info->power);
          }
          break;
        case ONEWIRE_READ: 
          {
            int numBytes = (argv[2] & 0x7F + argv[3] << 7) & 0x3FFF;
            Serial.write(START_SYSEX);
            Serial.write(ONEWIRE_REPLY);
            Serial.write(pin);
            Serial.write(ONEWIRE_READ);
            for (int i=0;i<8;i++) {
              byte addr = info->addr[i];
              Serial.write(addr & 0b01111111);
              Serial.write((byte)((addr << 7) & 0b01111111));
            }
            for (int i=0;i<numBytes;i++) {
              byte value = device->read();
              Serial.write(value & 0b01111111);
              Serial.write((byte)((value << 7) & 0b01111111));
            }  
            Serial.write(END_SYSEX);
            break;
          }
        }
      }
      break;
    }
  case ONEWIRE_CONFIG: 
    {
      byte pin = argv[0];
      boolean power = argv[1];
      oneWireConfig(pin,power);
      break;
    }
  }
}



void setPinModeCallback(byte pin, int mode)
{
  pinState[pin] = 0;
  switch(mode) {
  case ONEWIRE:
    oneWireConfig(pin,POWER);
    break;
  default:
    Firmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
  }
  // TODO: save status to EEPROM here, if changed
}

void systemResetCallback() {
  for (int i=0;i<TOTAL_PINS;i++) {
    pinConfig[i] = 0;
    pinState[i] = 0;
    pinOneWire[i].device=NULL;
    for (int j=0;j<8;j++) {
      pinOneWire[i].addr[j]=0;
    }
    pinOneWire[i].power=false;
  }
}

void setup()
{
  Firmata.setFirmwareVersion(2, 4);

  Firmata.attach(SET_PIN_MODE, setPinModeCallback);
  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
  //setPinModeCallback(10,ONEWIRE);
  //oneWireConfig(10,1);
  //ow_device_info info = pinOneWire[10];
  //info.device = &ds;
  //info.power = 1;
}

void loop()
{
  //byte sysexSearch[2];
  //sysexSearch[0] = 10;
  //sysexSearch[1] = ONEWIRE_SEARCH;
  //sysexCallback(ONEWIRE_REQUEST,2,sysexSearch);
  while (Firmata.available()) {
     Firmata.processInput();
  }
}



