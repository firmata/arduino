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


#include <RotaryEncoderFirmata.h>

RotaryEncoderFirmata::RotaryEncoderFirmata()
{
    encoderPosition = 0;
    clicks = 0 ;
    encoderPresent = false ;
}

boolean RotaryEncoderFirmata::handleSysex(byte command, byte argc, byte *argv)
{

    if (command == ENCODER_CONFIG) {
        // are the pins digital pins?
        encoderPin1 = argv[0] ;
        encoderPin2 = argv[1] ;

        if (IS_PIN_DIGITAL(encoderPin1) &&
                IS_PIN_DIGITAL(encoderPin2)) {
            // pins are valid, now test to see if we already have an encoder present
            if(encoderPresent == false) {
                // make sure pull-ups are off for these pins these pins
                pinMode(encoderPin1, INPUT);
                digitalWrite(encoderPin1, HIGH);
                pinMode(encoderPin2, INPUT);
                digitalWrite(encoderPin2, HIGH);

                // instantiate the encoder
                theEncoder = new RotaryEncoder('a', encoderPin1, encoderPin2) ;
                encoderPresent = true ;
            }
            return true ;
        }
        // either illegal command or pins not digital
    }
    return false;
}

void RotaryEncoderFirmata::report()
{
    if( encoderPresent == true)
    {
        // read encoder data and return it
        encoderMSB = 0 ;
        encoderLSB = 0 ;
        clicks = 0 ;

        RotaryEncoder *encoder = NULL;
        encoder = RotaryEncoder::genie() ;
        if( encoder != NULL) {
            clicks = encoder->query() ;
            if (clicks > 0) {
                encoderPosition += clicks ;
            }
            if (clicks < 0) {
                encoderPosition += clicks ;
            }
        }

        encoderLSB = encoderPosition & 0x7f ;
        encoderMSB = (encoderPosition >> 7) & 0x7f ;

        Firmata.write(START_SYSEX);
        Firmata.write(ENCODER_DATA) ;
        Firmata.write(encoderPin1) ;
        Firmata.write(encoderLSB) ;
        Firmata.write(encoderMSB) ;
        Firmata.write(END_SYSEX);
    }
}


void RotaryEncoderFirmata::handleCapability(byte pin)
{
    return ;
}


boolean RotaryEncoderFirmata::handlePinMode(byte pin, int mode)
{
    return false ;
}

void RotaryEncoderFirmata::reset()
{
    return ;
}


