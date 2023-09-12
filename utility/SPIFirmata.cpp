/*
  SPIFirmata.cpp
  Copyright (C) 2017 Jeff Hoefs. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated September 23rd, 2017
*/

#include "SPIFirmata.h"

SPIFirmata::SPIFirmata()
{
  init();
}

void SPIFirmata::init()
{
  mDeviceId = 0;
  mCsPin = -1;
  mCsActiveState = SPI_CS_ACTIVE_LOW; // default
}

bool SPIFirmata::handlePinMode(uint8_t pin, int mode)
{
  // There is no reason for a user to manually set the SPI pin modes
  return false;
}

void SPIFirmata::handleCapability(uint8_t pin)
{
  // ignore SS pin for now
  if (IS_PIN_SPI(pin) && pin != SS) {
    Firmata.write(PIN_MODE_SPI);
    // would actually use a value that corresponds to a specific pin (MOSI, MISO, SCK)
    // for now, just set to 1
    Firmata.write(1);
  }
}

bool SPIFirmata::handleSysex(uint8_t command, uint8_t argc, uint8_t *argv)
{
  if (command == SPI_DATA) {
    uint8_t mode = argv[0];
    // Not using channel yet since the Arduino SPI API currently exposes only one channel.
    uint8_t channel = argv[1] & SPI_CHANNEL_MASK;

    switch (mode) {
      case SPI_BEGIN:
        SPI.begin();
        // SPI pin states are configured by SPI.begin, but we still register them with Firmata.
        Firmata.setPinMode(MOSI, PIN_MODE_SPI);
        Firmata.setPinMode(MISO, PIN_MODE_SPI);
        Firmata.setPinMode(SCK, PIN_MODE_SPI);
        // Ignore SS for now.
        //Firmata.setPinMode(SS, PIN_MODE_SPI);
        break;
      case SPI_BEGIN_TRANSACTION:
      {
        mDeviceId = argv[1] >> 2;
        uint8_t bitOrder = argv[2] & SPI_BIT_ORDER_MASK;
        uint8_t dataMode = argv[2] >> 1;
        uint32_t clockSpeed = (uint32_t)argv[3] | ((uint32_t)argv[4] << 7) |
            ((uint32_t)argv[5] << 14) | ((uint32_t)argv[6] << 21) | ((uint32_t)argv[7] << 28);

        // argv[8] = wordSize, but not currently used since SPI.transfer only uses 8-bit words

        if (argc > 9) {
          mCsPin = argv[9];
          pinMode(mCsPin, OUTPUT);

          if (argv[10] != END_SYSEX) {
            mCsActiveState = argv[10] & 0x01;
          } else {
            // Set default
            mCsActiveState = SPI_CS_ACTIVE_LOW;
          }
          // Set CS pin to opposite of active state
          digitalWrite(mCsPin, !mCsActiveState);

          // TODO - determine if we need to protect the CS pin.
          // If PIN_MODE_SPI is set like this, the user cannot manually control the CS pin
          // using DIGITAL_MESSAGE.
          // Firmata.setPinMode(mCsPin, PIN_MODE_SPI);
        }

        SPISettings settings(clockSpeed, getBitOrder(bitOrder), getDataMode(dataMode));
        SPI.beginTransaction(settings);
        break;
      }
      case SPI_END_TRANSACTION:
        SPI.endTransaction();
        break;
      case SPI_TRANSFER:
      {
        uint8_t csPinControl = argv[2];
        uint8_t numBytes = argv[3];

        if (mCsPin >= 0) setCsPinState(csPinControl, true);

        transfer(channel, numBytes, argc, argv);

        if (mCsPin >= 0) setCsPinState(csPinControl, false);

        break; // SPI_TRANSFER
      }
      case SPI_WRITE:
      {
        uint8_t csPinControl = argv[2];
        uint8_t numBytes = argv[3];

        if (mCsPin >= 0) setCsPinState(csPinControl, true);

        writeOnly(channel, numBytes, argc, argv);

        if (mCsPin >= 0) setCsPinState(csPinControl, false);

        break; // SPI_WRITE
      }
      case SPI_READ:
      {
        uint8_t csPinControl = argv[2];
        uint8_t numBytes = argv[3];

        if (mCsPin >= 0) setCsPinState(csPinControl, true);

        readOnly(channel, numBytes);

        if (mCsPin >= 0) setCsPinState(csPinControl, false);

        break; // SPI_READ
      }
      case SPI_END:
        SPI.end();
        break;
    } // end switch
    return true;
  }
  return false;
}

void SPIFirmata::reset()
{
  init();
}

void SPIFirmata::setCsPinState(uint8_t pinControl, bool start)
{
  bool csStartOnly = false;
  bool csEndOnly = false;

  if (pinControl == SPI_CS_DISABLE) {
    return;
  }
  if (pinControl == SPI_CS_START_ONLY) {
    csStartOnly = true;
  } else if (pinControl == SPI_CS_END_ONLY) {
    csEndOnly = true;
  }
  
  // Evaluate whether or not to set the active state at the start or end of the transfer.
  if (start && !csEndOnly) {
    digitalWrite(mCsPin, mCsActiveState);
  } else if (!start && !csStartOnly) {
    digitalWrite(mCsPin, !mCsActiveState);
  }
}

void SPIFirmata::transfer(uint8_t channel, uint8_t numBytes, uint8_t argc, uint8_t *argv)
{
  uint8_t offset = 4; // mode + channel + opts + numBytes
  uint8_t buffer[numBytes];
  uint8_t bufferIndex = 0;
  if (numBytes * 2 != argc - offset) {
    // TODO - handle error
    Firmata.sendString("SPI transfer fails numBytes test");
  }
  for (uint8_t i = 0; i < numBytes * 2; i += 2) {
    bufferIndex = (i + 1) / 2;
    buffer[bufferIndex] = argv[i + offset + 1] << 7 | argv[i + offset];
  }
  // During the transfer, the received buffer data is stored in the buffer in-place.
  SPI.transfer(buffer, numBytes);

  reply(channel, numBytes, buffer);
}

void SPIFirmata::writeOnly(uint8_t channel, uint8_t numBytes, uint8_t argc, uint8_t *argv)
{
  uint8_t offset = 4; // mode + channel + opts + numBytes
  uint8_t txValue;
  if (numBytes * 2 != argc - offset) {
    // TODO - handle error
    Firmata.sendString("SPI write fails numBytes test");
  }
  for (uint8_t i = 0; i < numBytes * 2; i += 2) {
    txValue = argv[i + offset + 1] << 7 | argv[i + offset];
    // TODO - consider using SPI.transfer(buffer, size)
    SPI.transfer(txValue);
  }
}

void SPIFirmata::readOnly(uint8_t channel, uint8_t numBytes)
{
  uint8_t replyData[numBytes];
  for (uint8_t i = 0; i < numBytes; i++) {
    replyData[i] = SPI.transfer(0x00);
  }
  reply(channel, numBytes, replyData);
}

void SPIFirmata::reply(uint8_t channel, uint8_t numBytes, uint8_t *buffer)
{
  Firmata.write(START_SYSEX);
  Firmata.write(SPI_DATA);
  Firmata.write(SPI_REPLY);
  Firmata.write(mDeviceId << 2 | channel);
  Firmata.write(numBytes);

  for (uint8_t i = 0; i < numBytes; i++) {
    Firmata.write(buffer[i] & 0x7F);
    Firmata.write(buffer[i] >> 7 & 0x7F);
  }

  Firmata.write(END_SYSEX);
}
