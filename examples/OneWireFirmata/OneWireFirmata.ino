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
#include "OneWireFirmata.h"

#define ONEWIRE_REQUEST 0x60
#define ONEWIRE_REPLY 0x61

#define ONEWIRE_SEARCH 0
#define ONEWIRE_SKIP_AND_WRITE 1
#define ONEWIRE_SKIP_AND_READ 2
#define ONEWIRE_SELECT_AND_WRITE 3
#define ONEWIRE_SELECT_AND_READ 4
#define ONEWIRE_READ 5
#define ONEWIRE_CONFIG 6
#define ONEWIRE_REPORT_CONFIG 7

#define POWER 1

// 1-Wire Pinmode
#define ONEWIRE 0x07

#define MAX_OW_QUERY 10

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* pins configuration */
byte pinConfig[TOTAL_PINS];         // configuration of every pin
int pinState[TOTAL_PINS];           // any value that has been written. (Does this make sense for OneWire?)

void sendValueAsTwo7bitBytes(int value) // is private in Firmata...
{
  Serial.write(value & B01111111); // LSB
  Serial.write(value >> 7 & B01111111); // MSB
}

int getValueFromTwo7bitBytes(byte *argv) {
  return (argv[0] & B01111111) + (argv[1] << 7);
}

ow_pin_config pinOneWire[TOTAL_PINS];
ow_report_config *owQueries[MAX_OW_QUERY];
int owQueryIndex = -1;
long previousMillis;
long samplingInterval = 5000; // defaults to 5 sec for now

void oneWireConfig(byte pin, boolean power) {
  pinConfig[pin] = ONEWIRE;
  ow_pin_config *owInfo = &pinOneWire[pin];
  if (owInfo->ow==NULL) {
    owInfo->ow = new OneWire(pin);
  }
  owInfo->pin = pin;
  owInfo->power = power;
};

void readAddress14Bit(byte *newAddress, byte *data) {
  for (byte i=0;i<8;i++) {
    newAddress[i] = getValueFromTwo7bitBytes(&data[i<<1]);
  }
};

void readAndReportOWData(byte pin, byte *addr, OneWire *ow, int numBytes) {
  Serial.write(START_SYSEX);
  Serial.write(ONEWIRE_REPLY);
  Serial.write(pin);
  Serial.write(ONEWIRE_READ);
  for (int i=0;i<8;i++) {
    if (addr) {
      sendValueAsTwo7bitBytes(addr[i]);
    } 
    else {
      Serial.write(0x00);
    }
  }
  for (int i=0;i<numBytes;i++) {
    sendValueAsTwo7bitBytes(ow->read());
  }  
  Serial.write(END_SYSEX);
};

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/

void sysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case ONEWIRE_REQUEST: 
    {
      byte pin = argv[0];
      ow_pin_config *pinConfig = &pinOneWire[pin];
      byte owCommand = argv[1];
      OneWire *ow = pinConfig->ow;
      if (ow) {
        switch(owCommand) {
        case ONEWIRE_SEARCH: 
          {
            ow->reset_search();
            Serial.write(START_SYSEX);
            Serial.write(ONEWIRE_REPLY);
            Serial.write(pin);
            Serial.write((byte)ONEWIRE_SEARCH);
            byte addrArray[8];
            while (ow->search(addrArray)) {
              for (int i=0;i<8;i++) {
                sendValueAsTwo7bitBytes(addrArray[i]);
              }
            }
            Serial.write(END_SYSEX);
            break;
          }
        case ONEWIRE_SKIP_AND_WRITE:
          {
            if (argc >= 5) {
              ow->reset();
              ow->skip();
              int numBytes = getValueFromTwo7bitBytes(&argv[2]) & 0x3FFF;
              for (int i=0;i<numBytes;i++) {
                ow->write(getValueFromTwo7bitBytes(&argv[(i<<1)+4]) & 0xFF,pinConfig->power);
              }
            }
            break;
          }
        case ONEWIRE_SKIP_AND_READ:
          {
            if (argc>= 6) {
              byte readCommand = getValueFromTwo7bitBytes(&argv[2]) &0xFF;
              int numBytes = getValueFromTwo7bitBytes(&argv[4]) & 0x3FFF;
              ow->reset();
              ow->skip();
              ow->write(readCommand,pinConfig->power);
              readAndReportOWData(pin, NULL, ow, numBytes);
            }
            break;
          }
        case ONEWIRE_SELECT_AND_WRITE: // PIN,COMMAND,ADDRESS,NUMBYTES,DATA
          {
            if (argc >= 20) {
              byte addr[8];
              readAddress14Bit(addr,&argv[2]); //2-17
              int numBytes = getValueFromTwo7bitBytes(&argv[18]) & 0x3FFF; //18-19
              ow->reset();
              ow->select(addr);
              for (int i=0;i<numBytes;i++) { //20...
                ow->write(getValueFromTwo7bitBytes(&argv[(i<<1)+20]) & 0xFF,pinConfig->power);
              }
            }
            break;
          }
        case ONEWIRE_SELECT_AND_READ: // PIN,COMMAND,ADDRESS,READCOMMAND,NUMBYTES
          {
            if (argc>= 22) {
              byte addr[8];
              readAddress14Bit(addr,&argv[2]); //2-17
              byte readCommand = getValueFromTwo7bitBytes(&argv[18]) &0xFF; //18-19
              int numBytes = getValueFromTwo7bitBytes(&argv[20]) & 0x3FFF; //20-21
              ow->reset();
              ow->select(addr);
              ow->write(readCommand,pinConfig->power);
              readAndReportOWData(pin, addr, ow, numBytes);
            }
            break;
          }
        case ONEWIRE_CONFIG: 
          {
            boolean power = argv[2];
            oneWireConfig(pin,power);
            break;
          }

          // struct ow_report_config {
          //   byte addr[8];
          //   byte preReadCommand;
          //   long readDelay;
          //   byte readCommand;
          //   int  numBytes;
          //   ow_pin_config *pinConfig;
          // }
        case ONEWIRE_REPORT_CONFIG:
          {
            if (argc>=26) {
              ow_report_config *config = new ow_report_config();
              readAddress14Bit(config->addr,&argv[2]); //2-17
              config->preReadCommand = getValueFromTwo7bitBytes(&argv[18]) &0xFF; //18-19
              config->readDelay = getValueFromTwo7bitBytes(&argv[20]) &0x3FFF; //20-21
              config->readCommand = getValueFromTwo7bitBytes(&argv[22]) &0xFF; //22-23
              config->numBytes = getValueFromTwo7bitBytes(&argv[24]) &0x3FFF; //24-25
              config->pinConfig = pinConfig;
              owQueries[owQueryIndex]=config;
              owQueryIndex++;
            }
          }
        }
      }
    }
    break;
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
}

void systemResetCallback() {
  for (int i=0;i<TOTAL_PINS;i++) {
    pinConfig[i] = 0;
    pinState[i] = 0;
    pinOneWire[i].ow=NULL;
    pinOneWire[i].power=false;
  }
  previousMillis = millis();
  if (owQueryIndex>-1) {
    for (byte i=0;i<=owQueryIndex;i++) {
      if (owQueries[i]) {
        delete owQueries[i];
        owQueries[i]=NULL;
      }
    }
    owQueryIndex=-1;
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
}

void loop()
{
  while (Firmata.available())
    Firmata.processInput();

  long currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    previousMillis += samplingInterval;
    // report OneWire data for all device with read continuous mode enabled
    if (owQueryIndex > -1) {
      for (byte i = 0; i <= owQueryIndex && i<MAX_OW_QUERY; i++) {
        ow_report_config *owConfig = owQueries[i];
        if (owConfig) {
          ow_pin_config *pinConfig = owConfig->pinConfig;  
          OneWire *ow = pinConfig->ow;
          ow->reset();
          ow->select(owConfig->addr);
          ow->write(owConfig->preReadCommand,pinConfig->power);
          delay(owConfig->readDelay); // TODO, this does block other tasks. Requires some basic scheduling (in Firmata?) to refactor
          ow->reset();
          ow->select(owConfig->addr);
          ow->write(owConfig->readCommand,pinConfig->power);
          readAndReportOWData(pinConfig->pin, owConfig->addr, ow, owConfig->numBytes);
        }
      }
    }
  }
}





