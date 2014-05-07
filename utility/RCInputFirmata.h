/*
  RCInputFirmata.h - Firmata library

  Version: DEVELOPMENT SNAPSHOT
  Date:    2014-05-06
  Author:  git-developer ( https://github.com/git-developer )
   
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef RCInputFirmata_h
#define RCInputFirmata_h

#include <utility/FirmataFeature.h>
#include <utility/RCOutputFirmata.h> // only as long as common constants are defined there
#include <RCSwitch.h>

/* Subcommands */
#define CONFIG_TOLERANCE        0x31
#define MESSAGE                 0x41

class RCInputFirmata:public FirmataFeature
{

public:
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();

private:

  /** 1 receiver per pin */
  RCSwitch* receivers[TOTAL_PINS];
  
  /** Initializes a receiver for the given pin */
  void attach(byte pin);
  
  /** Removes the receiver for the given pin */
  void detach(byte pin);

  /**
   * Extracts a tristate bit from a byte.
   *
   * @param tristateByte  A byte of 4 tristate bits
   * @param index         Index of the tristate bit to read (0..3)
   * @return Char representation of the requested tristate bit
   */
  char getTristateChar(byte tristateByte, byte index);

  /**
   * Sets a tristate bit within a byte.
   *
   * @param tristateByte  A byte of 4 tristate bits
   * @param index         Index of the tristate bit to write (0..3)
   * @param char          Tristate bit to write
   * @return The given byte with the requested tristate bit set
   */
  byte setTristateBit(byte tristateByte, byte index, char tristateChar);

  /**
   * Send a message to the firmata host.
   *
   * @param pin        Pin that corresponds to the message
   * @param subcommand Details about the message
   *                     (see the constants defined above)
   * @param length     Message length
   * @param data       Message content
   */  
  void sendMessage(byte pin, byte subcommand, byte length, byte *data);

};

#endif
