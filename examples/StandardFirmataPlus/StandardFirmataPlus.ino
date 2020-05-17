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

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated August 17th, 2017
*/

#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>
#include <utility/ExampleStandardFirmataCommon.h>

/*
  README

  StandardFirmataPlus adds additional features that may exceed the Flash and
  RAM sizes of Arduino boards such as ATMega328p (Uno) and ATMega32u4
  (Leonardo, Micro, Yun, etc). It is best to use StandardFirmataPlus with higher
  memory boards such as the Arduino Mega, Arduino Due, Teensy 3.0/3.1/3.2.

  All Firmata examples that are appended with "Plus" add the following features:

  - Ability to interface with serial devices using UART, USART, or SoftwareSerial
    depending on the capatilities of the board.

  NOTE: In order to use SoftwareSerial with the Firmata Serial feature,
  StandardFirmataPlus must be compiled with Arduino v1.6.6 or newer.

  At the time of this writing, StandardFirmataPlus will still compile and run
  on ATMega328p and ATMega32u4-based boards, but future versions of this sketch
  may not as new features are added.
*/

// In order to use software serial, you will need to compile this sketch with
// Arduino IDE v1.6.6 or higher. Hardware serial should work back to Arduino 1.0.
#include "utility/SerialFirmata.h"

void setup()
{
  initFirmataCommonBegin();
  
  // Save a couple of seconds by disabling the startup blink sequence.
  Firmata.disableBlinkVersion();

  // to use a port other than Serial, such as Serial1 on an Arduino Leonardo or Mega,
  // Call begin(baud) on the alternate serial port and pass it to Firmata to begin like this:
  // Serial1.begin(57600);
  // Firmata.begin(Serial1);
  // However do not do this if you are using SERIAL_MESSAGE

  Firmata.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for ATmega32u4-based boards and Arduino 101
  }

  initFirmataCommonEnd();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  loopFirmataCommon();
}
