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
  Copyright (C) 2013 Alan Yorinks. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/


#include <ToneFirmata.h>

void ToneFirmata::toneRequest(byte pin, int frequency, int duration)
{
    if (IS_PIN_DIGITAL(pin)) {
      tone(pin, frequency, duration) ;
    }
}

boolean ToneFirmata::handleSysex(byte command, byte argc, byte *argv)
{
    byte pin ;
    int frequency ;
    int duration ;

  if (command == TONE_REQUEST) {
      pin = argv[0];
      frequency = argv[1] + (argv[2] << 7);
      duration = argv[3] + (argv[4] << 7);

      toneRequest(pin, frequency, duration);
      return true ;
  }
  return false;
}

void ToneFirmata::handleCapability(byte pin)
{
    return ;
}


boolean ToneFirmata::handlePinMode(byte pin, int mode)
{
    return false ;
}

void ToneFirmata::reset()
{
    return ;
}
