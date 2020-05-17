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

  Last updated April 15th, 2018
*/

#include <Servo.h>
#include <Wire.h>
#include "utility/ExampleStandardFirmataCommon.h"

//#define SERIAL_DEBUG
#include "utility/firmataDebug.h"

/*
 * Uncomment the following include to enable interfacing
 * with Serial devices via hardware or software serial.
 */
// In order to use software serial, you will need to compile this sketch with
// Arduino IDE v1.6.6 or higher. Hardware serial should work back to Arduino 1.0.
//#include "utility/SerialFirmata.h"

// follow the instructions in bleConfig.h to configure your BLE hardware
#include "bleConfig.h"

void setup()
{
  DEBUG_BEGIN(9600);

  initFirmataCommonBegin();
  
  stream.setLocalName(FIRMATA_BLE_LOCAL_NAME);

#ifdef FIRMATA_BLE_ADVERTISING_INTERVAL
  // set the BLE advertising interval
  stream.setAdvertisingInterval(FIRMATA_BLE_ADVERTISING_INTERVAL);
#endif
  // set the BLE connection interval - this is the fastest interval you can read inputs
  stream.setConnectionInterval(FIRMATA_BLE_MIN_INTERVAL, FIRMATA_BLE_MAX_INTERVAL);
  // set how often the BLE TX buffer is flushed (if not full)
  stream.setFlushInterval(FIRMATA_BLE_TXBUFFER_FLUSH_INTERVAL);

#ifdef IS_IGNORE_BLE_PINS
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (IS_IGNORE_BLE_PINS(i)) {
      Firmata.setPinMode(i, PIN_MODE_IGNORE);
    }
  }
#endif

  stream.begin();
  Firmata.begin(stream);

  initFirmataCommonEnd();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  // do not process data if no BLE connection is established
  // poll will send the TX buffer at the specified flush interval or when the buffer is full
  if (!stream.poll()) return;

  loopFirmataCommon(true /* BLE specific */);
}
