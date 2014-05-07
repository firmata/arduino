/*
  RCInputFirmata.cpp - Firmata library

  Version: 1.0-SNAPSHOT
  Date:    2014-05-06
  Author:  git-developer ( https://github.com/git-developer )

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "RCInputFirmata.h"
#include <Encoder7Bit.h>
#define NO_INTERRUPT -1

void RCInputFirmata::handleCapability(byte pin)
{
  if (IS_PIN_INTERRUPT(pin)) {
    Firmata.write(RC_RECEIVE);
    Firmata.write(1); // data doesn't have a fixed resolution
  }
}

boolean RCInputFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_INTERRUPT(pin)) {
    if (mode == RC_RECEIVE) {
      return attach(pin);
    } else {
      detach(pin);
    }
  }
  return false;
}

void RCInputFirmata::reset()
{
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_INTERRUPT(pin)) {
      detach(pin);
    }
  }
}

boolean RCInputFirmata::handleSysex(byte command, byte argc, byte *argv)
{
  return false;
}

void RCInputFirmata::report()
{
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_INTERRUPT(pin)) {
      RCSwitch *receiver = receivers[pin];
      if (receiver && receiver->available()) {
        long value    = receiver->getReceivedValue();
        int bitlength = receiver->getReceivedBitlength();
        int delay     = receiver->getReceivedDelay();
        int protocol  = receiver->getReceivedProtocol();
        unsigned int *rawdata  = receiver->getReceivedRawdata();
        receiver->resetAvailable();
       
        byte data[10];
        data[0] = (value >> 24) & 0xFF;
        data[1] = (value >> 16) & 0xFF;
        data[2] = (value >>  8) & 0xFF;
        data[3] = value & 0xFF;
        
        data[4] = (bitlength >> 8) & 0xFF;
        data[5] = bitlength & 0xFF;
        
        data[6] = (delay >> 8) & 0xFF;
        data[7] = delay & 0xFF;
        
        data[8] = (protocol >> 8) & 0xFF;
        data[9] = protocol & 0xFF;
  
        sendMessage(MESSAGE, pin, 10, data);
      }
    }
  }
}

boolean RCInputFirmata::attach(byte pin)
{
  int interrupt = getInterrupt(pin);
  if (interrupt == NO_INTERRUPT) {
    return false;
  }
  RCSwitch *receiver = receivers[pin];
  if (!receiver) {
    receiver = new RCSwitch();
    receivers[pin] = receiver;
  }
  receiver->enableReceive(interrupt); 
}

void RCInputFirmata::detach(byte pin)
{
  RCSwitch *receiver = receivers[pin];
  if (receiver) {
    receiver->disableReceive();
    free(receiver);
    receivers[pin]=NULL;
  }
}

void RCInputFirmata::sendMessage(byte subcommand, byte pin, byte length, byte *data)
{
  Firmata.write(START_SYSEX);
  Firmata.write(RC_DATA);
  Firmata.write(subcommand);
  Firmata.write(pin);
  Encoder7Bit.startBinaryWrite();
  for (int i = 0; i < length; i++) {
    Encoder7Bit.writeBinary(data[i]);
  }
  Encoder7Bit.endBinaryWrite();
  Firmata.write(END_SYSEX);
}

byte RCInputFirmata::getInterrupt(byte pin) { //TODO this only fits Arduino Mega; check how this can be made more flexible to fit different boards
  byte interrupt = NO_INTERRUPT;
  switch(pin) {
    case   2: interrupt = 0; break;
    case   3: interrupt = 1; break;
    case  21: interrupt = 2; break;
    case  20: interrupt = 3; break;
    case  19: interrupt = 4; break;
    case  18: interrupt = 5; break;
  }
  return interrupt;
}
