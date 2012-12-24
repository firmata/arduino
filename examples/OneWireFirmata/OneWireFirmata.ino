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

#include "Boards.h"
#include "Firmata.h"
#include "FirmataExt.h"
#include "OneWire.h"
#include "OneWireScheduler.h"

#define ONEWIRE_REQUEST 0x60
#define ONEWIRE_REPLY 0x61

#define POWER 1

// 1-Wire Pinmode
#define ONEWIRE 0x07

#define ONEWIRE_CONFIG_REQUEST 0
#define ONEWIRE_SEARCH_REQUEST 1
#define ONEWIRE_TASK_REQUEST 2
#define ONEWIRE_RESET_REQUEST 3
#define ONEWIRE_SEARCH_REPLY 4
#define ONEWIRE_READ_REPLY 5

//byte testcommands[]={
//  ONEWIRE_RESET_COMMAND,
//  ONEWIRE_SELECT_COMMAND,
//  ONEWIRE_WRITE_COMMAND,0x01,0x00,0x44,
//  ONEWIRE_DELAY_COMMAND,0xE8,0x03,0x00,0x00,
//  ONEWIRE_RESET_COMMAND,
//  ONEWIRE_SELECT_COMMAND,
//  ONEWIRE_WRITE_COMMAND,0x01,0x00,0xBE,
//  ONEWIRE_READ_COMMAND,0x09,0x00,
//  ONEWIRE_RESCHEDULE_COMMAND,0x88,0x13,0x00,0x00
//};
//
//OneWire ds = OneWire(10);
//
//ow_config testconfig={
//  10,&ds,1};
//byte testaddr[]={
//  0x28,0xA0,0x89,0xAD,0x03,0x00,0x00,0xAA};


/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* pins configuration */
byte pinConfig[TOTAL_PINS];         // configuration of every pin
ow_config pinOneWire[TOTAL_PINS];

OneWireScheduler oneWireScheduler(&oneWireCallback);

byte lastCommand;

void sendValueAsTwo7bitBytes(int value) // is private in Firmata...
{
  Serial.write(value & B01111111); // LSB
  Serial.write(value >> 7 & B01111111); // MSB
}

void sendBytes14Bit(byte *data, int numBytes)
{
  for (byte i=0;i<numBytes;i++) {
    sendValueAsTwo7bitBytes(data[i]);
  }
}

int getValueFromTwo7bitBytes(byte *argv) {
  return (argv[0] & B01111111) + (argv[1] << 7);
}

void readBytes14Bit(byte *newData, byte *data, int numBytes) {
  for (byte i=0;i<numBytes;i++) {
    newData[i] = getValueFromTwo7bitBytes(&data[i<<1]);
  }
};

//numBytes<=27 -> messageSize <= 31
void sendBinarySysex(byte command, byte numBytes, byte *data) {
  byte messageSize = (numBytes<<3)/7;
  if ((numBytes<<8)%7>0) {
    messageSize++;
  }
  Serial.write(START_SYSEX);
  Serial.write(command);
  for (byte i=0;i<messageSize;i++) {
    byte j = i*7;
    byte pos=j>>3;   // 0,0,1,2,3,4,5,6,7,7,8...
    byte shift=j&7;  // 0,7,6,5,4,3,2,1,0,7,6...
    byte out = (data[pos]>>shift) & 0x7F;
    if (shift>1 && pos<(numBytes-1)) {
      out|=(data[pos+1]<<(8-shift))&0x7F;
    }
    Serial.write(out);
  }
  Serial.write(END_SYSEX);
}

byte readBinarySysex(byte *outData, byte *inData, byte numInBytes) {
  byte outBytes=(numInBytes*7)>>3;
  for (byte i=0;i<outBytes;i++) {
    byte j=i<<3;
    byte pos=j/7;
    byte shift=j%7;
    outData[i]=(inData[pos]>>shift)|((inData[pos+1]<<(7-shift))&0xFF);
  }
  return outBytes;
}

void oneWireConfig(byte pin, boolean power) {
  pinConfig[pin] = ONEWIRE;
  ow_config *config = &pinOneWire[pin];
  if (config->ow==NULL) {
    config->ow = new OneWire(pin);
  }
  config->pin = pin;
  config->power = power;
};

void echo(byte command, int argc, byte* argv) {
  Serial.write(START_SYSEX);
  Serial.write(command);
  for (int i=0;i<argc;i++) {
    Serial.write(argv[i]);
  }
  Serial.write(END_SYSEX);
}

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/

void sysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case ONEWIRE_REQUEST: 
    {
      FirmataExt.sysexCallback(command,argc,argv);
    }
  }
}

void decodedSysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case ONEWIRE_REQUEST: 
    {
      byte pin = argv[0];
      ow_config *pinConfig = &pinOneWire[pin];
      byte owCommand = argv[1];
      OneWire *ow = pinConfig->ow;
      if (ow) {
        switch(owCommand) {
        case ONEWIRE_CONFIG_REQUEST: 
          {
            boolean power = argv[2];
            oneWireConfig(pin,power);
            break;
          }
        case ONEWIRE_SEARCH_REQUEST:
          {
            Serial.write(START_SYSEX);
            Serial.write(ONEWIRE_REPLY);
            Serial.write(pin);
            Serial.write((byte)ONEWIRE_SEARCH_REPLY);
            byte addr[8];
            while(ow->search(addr)) {
              for (byte i=0;i<8;i++) {
                sendValueAsTwo7bitBytes(addr[i]);
              }
            }
            Serial.write(END_SYSEX);
            break;
          }
        case ONEWIRE_TASK_REQUEST:
          {
            if (argc>=10) { //PIN,COMMAND,ADDR,COMMANDS
              oneWireScheduler.schedule(pinConfig,argv+2,argc-10,argv+10,millis());   
            }
            break;
          }
        case ONEWIRE_RESET_REQUEST:
          {
            oneWireScheduler.reset();
            break;
          }
        }
      }
      break;
    }
  }
}

void setPinModeCallback(byte pin, int mode)
{
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
    if (pinOneWire[i].ow) {
      delete pinOneWire[i].ow;
      pinOneWire[i].ow=NULL;
    }
    pinOneWire[i].power=false;
  }
  lastCommand = ONEWIRE_DONE_COMMAND;
  oneWireScheduler.reset();
  // oneWireScheduler.schedule(&testconfig,testaddr, testcommands, millis());
}

void setup()
{
  Firmata.setFirmwareVersion(2, 4);

  Firmata.attach(SET_PIN_MODE, setPinModeCallback);
  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  FirmataExt.attach(decodedSysexCallback);

  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
}

void loop()
{
  while (Firmata.available())
    Firmata.processInput();
  oneWireScheduler.runTasks();
}

void oneWireCallback(byte command,ow_task *task,byte data) {
  switch(command) {
  case ONEWIRE_SEARCH_COMMAND: 
    {
      if (lastCommand == ONEWIRE_DONE_COMMAND) {
        Serial.write(START_SYSEX);
        Serial.write(ONEWIRE_REPLY);
        Serial.write(task->config->pin);
        Serial.write((byte)ONEWIRE_SEARCH_REPLY);
        lastCommand=ONEWIRE_SEARCH_COMMAND;
      }
      sendBytes14Bit(task->addr,8);
      break;
    }
  case ONEWIRE_READ_COMMAND: 
    {
      if (lastCommand == ONEWIRE_DONE_COMMAND) {
        Serial.write(START_SYSEX);
        Serial.write(ONEWIRE_REPLY);
        Serial.write(task->config->pin);
        Serial.write((byte)ONEWIRE_READ_REPLY);
        sendBytes14Bit(task->addr,8);
        lastCommand=ONEWIRE_READ_COMMAND;
      }
      sendValueAsTwo7bitBytes(data);
      break;
    }
  case ONEWIRE_DONE_COMMAND: 
    {
      Serial.write(END_SYSEX);
      lastCommand=ONEWIRE_DONE_COMMAND;
      break;
    }
  }
}




