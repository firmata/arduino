/*
  RCInputFirmata.h - Firmata library

  Version: 1.0-SNAPSHOT
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
#include <RCSwitch.h>

/* Subcommands */
#define UNKNOWN                 0x00
#define CONFIG_TOLERANCE        0x31
#define MESSAGE                 0x41

class RCInputFirmata:public FirmataFeature
{

public:
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();
  void report();

private:

  /** 1 receiver per pin */
  RCSwitch* receivers[TOTAL_PINS];
  
  /** Initializes a receiver for the given pin */
  boolean attach(byte pin);
  
  /** Removes the receiver for the given pin */
  void detach(byte pin);

  /**
   * Send a message to the firmata host.
   *
   * @param subcommand Details about the message
   *                     (see the constants defined above)
   * @param pin        Pin that corresponds to the message
   * @param length     Message length
   * @param data       Message content
   */  
  void sendMessage(byte subcommand, byte pin, byte length, byte *data);

  byte getInterrupt(byte pin);

};

#endif
