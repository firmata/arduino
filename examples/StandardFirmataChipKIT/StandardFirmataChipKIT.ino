/*
  Firmata is a generic protocol for communicating with microcontrollers
  from software on a host computer. It is intended to work with
  any host computer software package.

  To download a host software package, please click on the following link
  to open the list of Firmata client libraries in your default browser.

  https://github.com/firmata/arduino#firmata-client-libraries

  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2015 Brian Schmalz. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated August 17th, 2017
*/

#include <SoftPWMServo.h>  // Gives us PWM and Servo on every pin
#include <Wire.h>

/* Sets a pin that is in Servo mode to a particular output value
 * (i.e. pulse width). Different boards may have different ways of
 * setting servo values, so putting it in a function keeps things cleaner.
 */
void servoWrite(byte pwmpin, int value)
{
  SoftPWMServoPWMWrite(pwm_pin, value);
}

#define SERVO SoftServo
#define ANALOGWRITE_PIN_MODE_PWM servoWrite
#undef FIRMATA_SERIAL_FEATURE

#include "utility/ExampleStandardFirmataCommon.h"

void setup()
{
  initFirmataCommonBegin();

  /* For chipKIT Pi board, we need to use Serial1. All others just use Serial. */
#if defined(_BOARD_CHIPKIT_PI_)
  Serial1.begin(57600);
  Firmata.begin(Serial1);
#else
  Firmata.begin(57600);
#endif

  initFirmataCommonEnd();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  loopFirmataCommon();
}
