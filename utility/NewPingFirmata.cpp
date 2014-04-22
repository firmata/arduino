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
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2013 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2014 Alan Yorinks. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/

#include <NewPingFirmata.h>
#include <Firmata.h>

boolean NewPingFirmata::handlePinMode(byte pin, int mode)
{
  if (mode == PING) {
    if (IS_PIN_DIGITAL(pin)) {
      return true;
    }
  }
  return false;
}

void NewPingFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write(PING);
    Firmata.write(14); //  ping distance in cm. maximum 14 bits
  }
}

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/


boolean NewPingFirmata::handleSysex(byte command, byte argc, byte *argv)
{
  uint8_t pingerTriggerPin;
  uint8_t pingerEchoPin;
  byte pingerMSB, pingerLSB;

  if( command == PING_DATA)
  {
    if ( numActivePingers < MAX_PINGERS)
    {
      pingerTriggerPin = argv[0];
      pingerEchoPin = argv[1];
      // set interval to a minium of 33 ms.
      if ( argv[2] >= 33 ) {
        pingInterval = argv[2];
      }
      else {
        pingInterval = 33;
      }
      pingPinNumbers[numActivePingers] = pingerTriggerPin;

      Firmata.setPinMode(pingerTriggerPin, PING);
      Firmata.setPinMode(pingerEchoPin, PING);
      if (numActivePingers == 0) {
        pingTimer = millis();
      }
      
      pingers[numActivePingers] =
        new NewPing(pingerTriggerPin, pingerEchoPin, pingInterval);
      numActivePingers++;
    }
    else {
      return false;
    }
  }
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void NewPingFirmata::reset()
{
  currentPinger = 0;
  numActivePingers=0;
  for (byte i=0; i<MAX_PINGERS; i++) {
    if (pingers[i]) {
      free(pingers[i]);
      pingers[i] = 0;
      pingPinNumbers[i] = IGNORE;
    }
  }
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void NewPingFirmata::update()
{
    uint8_t pingerLSB;
    uint8_t pingerMSB;
    unsigned int pingResult;

    if (numActivePingers)
    {
      unsigned int uS;
      unsigned long currentTime = millis();
      if( (currentTime - pingTimer) > pingInterval) {
        pingTimer = currentTime;
        // send next device ping
        uS = pingers[nextPinger]->ping();
        // Convert ping time to distance in cm and print
        pingResult = uS / US_ROUNDTRIP_CM;
        currentPinger = nextPinger;
        if ( nextPinger++ >= numActivePingers - 1)
        {
          nextPinger = 0;
        }
        pingerLSB = pingResult & 0x7f;
        pingerMSB = pingResult >> 7 & 0x7f;

        Firmata.write(START_SYSEX);
        Firmata.write(PING_DATA);
        Firmata.write(pingPinNumbers[currentPinger]);
        Firmata.write(pingerLSB);
        Firmata.write(pingerMSB);
        Firmata.write(END_SYSEX);
        }
    }
}
