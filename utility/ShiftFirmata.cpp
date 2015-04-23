/*
  ShiftFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2013 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include <Firmata.h>
#include <ShiftFirmata.h>
#include <Encoder7Bit.h>

boolean ShiftFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_DIGITAL(pin) && mode == SHIFT) {

    shift_info *info = &pinShift[pin];

    pinMode(PIN_TO_DIGITAL(info->clockPin), OUTPUT);

    if (info->latchPin) {
      pinMode(PIN_TO_DIGITAL(info->latchPin), OUTPUT);
    }

    switch (info->type) {
      case SHIFT_OUT:
      case LATCH_L_SHIFT_OUT:
      case SHIFT_OUT_LATCH_H:
      case LATCH_L_SHIFT_OUT_LATCH_H:
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable PWM
        pinMode(PIN_TO_DIGITAL(info->dataPin), OUTPUT);
        break;
      case SHIFT_IN:
      case TOGGLE_LOAD_SHIFT_IN:
        pinMode(PIN_TO_DIGITAL(info->dataPin), INPUT);
        break;
      default:
        Firmata.sendString("No shift type specified.");
        // default to shift out
        info->type = SHIFT_OUT;
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable PWM
        pinMode(PIN_TO_DIGITAL(info->dataPin), OUTPUT);        
        break;
    }
    return true;

  }
  return false;
}

void ShiftFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write(SHIFT);
    Firmata.write(8); //8-bit resolution. Can send and receive multiple bytes.
  }
}

boolean ShiftFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  if(command == SHIFT_DATA) {
    byte shiftCommand, dataPin, shiftType, shiftInData, shiftOutData;
    int numBytes;

    numBytes = 0;

    shiftCommand = argv[0] & SHIFT_COMMAND_MASK;
    dataPin = argv[1];

    shift_info *info = &pinShift[dataPin];

    /********************* SHIFT CONFIG *********************/
    if (shiftCommand == SHIFT_CONFIG) {
      shiftType = (argv[0] & SHIFT_TYPE_MASK) >> 3;

      info->type = shiftType;
      info->dataPin = dataPin;
      info->clockPin = argv[2];
      info->bitOrder = argv[3];

      if (shiftType > 3 && shiftType < 8) {
        info->latchPin = argv[4];
      } else {
        info->latchPin = NULL;
      }

      Firmata.setPinMode(dataPin, SHIFT);

    /********************* SHIFT OUT *********************/
    } else if (shiftCommand == SHIFT_OUT) {
      numBytes = num7BitOutbytes(argc - 2); // data starts after dataPin
      argv += 2;
      Encoder7Bit.readBinary(numBytes, argv, argv); // decode in place

      if (info->latchPin && info->type > 3) {
        switch (info->type) {
          case LATCH_L_SHIFT_OUT:
          case LATCH_L_SHIFT_OUT_LATCH_H:
            digitalWrite(info->latchPin, LOW);
            break;
        }
      }
      for (int i = 0; i < numBytes; i++) {
        shiftOutData = argv[i];
        shiftOut(dataPin, info->clockPin, info->bitOrder, shiftOutData);
      }
      if (info->latchPin && info->type > 3) {
        switch (info->type) {
          case SHIFT_OUT_LATCH_H:
          case LATCH_L_SHIFT_OUT_LATCH_H:
            digitalWrite(info->latchPin, HIGH);
            break;
        }
      }

    /********************* SHIFT IN *********************/  
    } else if (shiftCommand == SHIFT_IN) {
      numBytes = argv[2];

      if (info->latchPin && info->type == TOGGLE_LOAD_SHIFT_IN) {
        digitalWrite(info->latchPin, LOW);
        digitalWrite(info->latchPin, HIGH);
      }

      Firmata.write(START_SYSEX);
      Firmata.write(SHIFT_DATA);
      Firmata.write(SHIFT_IN_REPLY);
      Firmata.write(dataPin);
      Encoder7Bit.startBinaryWrite();
      for (int i = 0; i < numBytes; i++) {
        shiftInData = shift_In(dataPin, info->clockPin, info->bitOrder);
        Encoder7Bit.writeBinary(shiftInData);
      }
      Encoder7Bit.endBinaryWrite();
      Firmata.write(END_SYSEX);
    }
    return true;
  }
  return false;
}

// The built-in arduino shiftIn function doesn't work so we need our own.
byte ShiftFirmata::shift_In(byte dataPin, byte clockPin, byte bitOrder)
{
  byte value = 0;
  byte i;

  for (i = 0; i < 8; i++) {
    if (bitOrder == LSBFIRST)
      value |= digitalRead(dataPin) << i;
    else
      value |= digitalRead(dataPin) << (7 - i);

    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  return value;
}

void ShiftFirmata::reset()
{
  for (int i = 0; i < TOTAL_PINS; i++) {
    if (pinShift[i].latchPin) {
      pinShift[i].latchPin = NULL;
    }
  }
}
