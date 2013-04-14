/*
  ShiftFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
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
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == SHIFT) {
      // pin modes for data, clock and latch pins need to be set separately
      return true;
    }
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
    byte shiftCommand, dataPin, clockPin, latchPin, bitOrder;
    byte shiftInData, shiftOutData;
    int numBytes;

    numBytes = 0;

    shiftCommand = argv[0];
    dataPin = argv[1];
    clockPin = argv[2];
    latchPin = argv[3];
    bitOrder = argv[4];

    // set only the data pin to SHIFT
    if (Firmata.getPinMode(dataPin != SHIFT)) {
      Firmata.setPinMode(dataPin, SHIFT);
    }

    if (shiftCommand == SHIFT_OUT) {
      numBytes = num7BitOutbytes(argc - 5); // data starts after bitOrder (argv[4])
      argv += 5;
      Encoder7Bit.readBinary(numBytes, argv, argv); // decode in place

      for (int i = 0; i < numBytes; i++) {
        shiftOutData = argv[i];

        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, bitOrder, shiftOutData);
        digitalWrite(latchPin, HIGH);
      }

    } else if (shiftCommand == SHIFT_IN) {
      numBytes = argv[5];
      Firmata.write(START_SYSEX);
      Firmata.write(SHIFT_DATA);
      Firmata.write(SHIFT_IN_REPLY);
      Firmata.write(dataPin);
      Encoder7Bit.startBinaryWrite();
      for (int i = 0; i < numBytes; i++) {
        digitalWrite(latchPin, LOW); // load bits
        digitalWrite(latchPin, HIGH);

        shiftInData = shift_In(dataPin, clockPin, bitOrder);
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
  // to do: any necessary cleanup
}
