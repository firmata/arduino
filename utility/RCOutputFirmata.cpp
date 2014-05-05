/*
  RCOutputFirmata.cpp - Firmata library

  Version: DEVELOPMENT SNAPSHOT
  Date:    2014-05-03
  Author:  fhem-user ( http://forum.fhem.de/index.php?action=emailuser;sa=email;uid=1713 )

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "RCOutputFirmata.h"
#include <Encoder7Bit.h>

void RCOutputFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write(PINMODE_RC_TRANSMIT);
    Firmata.write(1); // data doesn't have a fixed resolution
  }
}

boolean RCOutputFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == PINMODE_RC_TRANSMIT) {
      attach(pin);
      return true;
    } else {
      detach(pin);
    }
  }
  return false;
}

void RCOutputFirmata::reset()
{
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_DIGITAL(pin)) {
      detach(pin);
    }
  }
}

boolean RCOutputFirmata::handleSysex(byte command, byte argc, byte *argv)
{
  /* required: pin, subcommand, value */
  if (command != SYSEX_COMMAND_RC_DATA || argc <= 2) {
    return false;
  }
  byte pin = argv[0];
  if (Firmata.getPinMode(pin) == IGNORE) {
    return false;
  }
  RCSwitch *sender = senders[pin];
  if (!sender) { // pin was not attached yet
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
  
  byte subcommand = argv[1];
  byte *data = (byte*) argv+2;
  Encoder7Bit.readBinary(length, data, data);
  int value = *(int*) data;

  switch (subcommand) {
    case CONFIG_PROTOCOL:        { sender->setProtocol(value); break; }
    case CONFIG_PULSE_LENGTH:    { sender->setPulseLength(value); break; }
    case CONFIG_REPEAT_TRANSMIT: { sender->setRepeatTransmit(value); break; }
    case CODE_TRISTATE: { 
      char tristateCode[length*4];
      byte charCount = unpack(data, length, tristateCode);
      sender->sendTriState(tristateCode);
      length = pack(tristateCode, charCount, data);
      break;
    }
    default: { subcommand = UNKNOWN; }
  }
  sendMessage(pin, subcommand, length, data);
  return true;
}

void RCOutputFirmata::attach(byte pin)
{
  RCSwitch *sender = senders[pin];
  if (!sender) {
    sender = new RCSwitch();
    senders[pin] = sender;
  }
  sender->enableTransmit(pin); 
}

void RCOutputFirmata::detach(byte pin)
{
  RCSwitch *sender = senders[pin];
  if (sender) {
    sender->disableTransmit();
    free(sender);
    senders[pin]=NULL;
  }
}

byte RCOutputFirmata::unpack(byte *tristateBytes, byte length, char* tristateCode)
{
  byte charCount = 0;
  for (byte i = 0; i < length; i++) {
    for (byte j = 0; j < 4; j++) {
      tristateCode[charCount++] = getTristateChar(tristateBytes[i], j);
    }
  }
  return charCount;
}

byte RCOutputFirmata::pack(char* tristateCode, byte length, byte *tristateBytes)
{
  byte count = 0;
  for (; count < length; count++) {
    tristateBytes[count/4] = setTristateBit(tristateBytes[count/4],
                                            count & 0x03,
                                            tristateCode[count]);
  }
  
  /* fill last byte if necessary */
  for (; (count & 0x03) != 0; count++) {
    tristateBytes[count/4] = setTristateBit(tristateBytes[count/4],
                                            count & 0x03,
                                            TRISTATE_RESERVED);
  }
  return count/4;
}

char RCOutputFirmata::getTristateChar(byte tristateByte, byte index) {

  /* 
   * An invalid character is used as default
   * so that no valid value will be used on error
   */
  char c = 'X'; // 
  byte shift = 2*(index & 0x03); // 0, 2, 4 or 6
  byte tristateBit = ((tristateByte << shift) >> 6) & 0x3;
  switch (tristateBit) {
    case TRISTATE_0: c = '0'; break;
    case TRISTATE_F: c = 'F'; break;
    case TRISTATE_1: c = '1'; break;
  }
  return c;
}

byte RCOutputFirmata::setTristateBit(byte tristateByte, byte index, char tristateChar) {
  byte shift = 6-(2*index); // 6, 4, 2 or 0
  byte clear = ~(0x03 << shift); // bitmask to clear the requested 2 bits
  byte tristateBit = TRISTATE_RESERVED;
  switch (tristateChar) {
    case '0': tristateBit = TRISTATE_0; break;
    case 'F': tristateBit = TRISTATE_F; break;
    case '1': tristateBit = TRISTATE_1; break;
  }
  
  /* remove the requested tristate bit and set it afterwards */
  return (tristateByte & clear) | (tristateBit << shift);
}

void RCOutputFirmata::sendMessage(byte pin, byte subcommand, byte length, byte *data)
{
  Firmata.write(START_SYSEX);
  Firmata.write(SYSEX_COMMAND_RC_DATA);
  Firmata.write(pin);
  Firmata.write(subcommand);
  Encoder7Bit.startBinaryWrite();
  for (int i = 0; i < length; i++) {
    Encoder7Bit.writeBinary(data[i]);
  }
  Encoder7Bit.endBinaryWrite();
  Firmata.write(END_SYSEX);
}
