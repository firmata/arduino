/*
  RCInputFirmata.cpp - Firmata library

  Version: 1.0-SNAPSHOT
  Date:    2014-08-23
  Author:  git-developer ( https://github.com/git-developer )

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "RCInputFirmata.h"
#include <Encoder7Bit.h>

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
  rawdataEnabled = false;
}

boolean RCInputFirmata::handleSysex(byte command, byte argc, byte *argv)
{
  /* required: subcommand, pin, value */
  if (command != RC_DATA || argc <= 2) {
    return false;
  }
  byte subcommand = argv[0];
  byte pin = argv[1];
  if (Firmata.getPinMode(pin) == IGNORE) {
    return false;
  }
  RCSwitch *receiver = receivers[pin];
  if (!receiver) { // pin was not attached yet
    return false;
  }
  
  /* 
   * argc gives the number of 7-bit bytes (control and data),
   * length is the number of 8-bit bytes (data only)
   */
  byte length = ((argc-2) * 7) >> 3;
  if (length == 0) {
    return false;
  }
  
  byte *data = (byte*) argv+2;
  Encoder7Bit.readBinary(length, data, data); // decode in-place
  int value = *(int*) data;

  switch (subcommand) {
   case CONFIG_TOLERANCE:       { receiver->setReceiveTolerance(value); break; }
   case CONFIG_ENABLE_RAW_DATA: { rawdataEnabled = (boolean) data[0]; break; }
   default:                     { subcommand = UNKNOWN; }
  }
  sendMessage(subcommand, pin, length, data, 0, data);
  return subcommand != UNKNOWN;
}

void RCInputFirmata::report()
{
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_INTERRUPT(pin)) {
      RCSwitch *receiver = receivers[pin];
      if (receiver && receiver->available()) {
        unsigned long value    = receiver->getReceivedValue();
        unsigned int bitCount  = receiver->getReceivedBitlength();
        unsigned int delay     = receiver->getReceivedDelay();
        unsigned int protocol  = receiver->getReceivedProtocol();
        
        /* 
         * Note: The rawdata values can be changed while we read them.
         * This cannot be avoided because rawdata is written from an interrupt
         * routine.
         */
        unsigned int *rawdata  = receiver->getReceivedRawdata();
        receiver->resetAvailable();

        byte data[10];
        data[0] = (value >> 24) & 0xFF;
        data[1] = (value >> 16) & 0xFF;
        data[2] = (value >>  8) & 0xFF;
        data[3] = value & 0xFF;
        
        data[4] = (bitCount >> 8) & 0xFF;
        data[5] = bitCount & 0xFF;
        
        data[6] = (delay >> 8) & 0xFF;
        data[7] = delay & 0xFF;
        
        data[8] = (protocol >> 8) & 0xFF;
        data[9] = protocol & 0xFF;
        
        byte rawdataLength = rawdataEnabled ? 2*RCSWITCH_MAX_CHANGES : 0;
        
        sendMessage(MESSAGE, pin, 10, data, rawdataLength, (byte*) rawdata);
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
  pinMode(PIN_TO_DIGITAL(pin), INPUT);
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

void RCInputFirmata::sendMessage(byte subcommand, byte pin,
                                 byte length1, byte *data1,
                                 byte length2, byte *data2)
{
  Firmata.write(START_SYSEX);
  Firmata.write(RC_DATA);
  Firmata.write(subcommand);
  Firmata.write(pin);
  Encoder7Bit.startBinaryWrite();
  for (int i = 0; i < length1; i++) {
    Encoder7Bit.writeBinary(data1[i]);
  }
  for (int i = 0; i < length2; i++) {
    Encoder7Bit.writeBinary(data2[i]);
  }
  Encoder7Bit.endBinaryWrite();
  Firmata.write(END_SYSEX);
}

byte RCInputFirmata::getInterrupt(byte pin) {
// this method fits common Arduino board including Mega.
// TODO check how this can be made more flexible to fit different boards

  byte interrupt = NO_INTERRUPT;
  switch (pin) {
    case   2: interrupt = 0; break;
    case   3: interrupt = 1; break;
    case  21: interrupt = 2; break;
    case  20: interrupt = 3; break;
    case  19: interrupt = 4; break;
    case  18: interrupt = 5; break;
  }
  return interrupt;
}
