/*
  AnalogWrite.h - Firmata library
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

#include "FirmataConfig.h"
#include <Firmata.h>
#include "AnalogWrite.h"

#if (defined ANALOGFIRMATA || defined SERVOFIRMATA)

#ifdef ANALOGFIRMATA
#include "AnalogFirmata.h"
#endif
#ifdef SERVOFIRMATA
#include "ServoFirmata.h"
#endif

void analogWriteCallback(byte pin, int value)
{
  if (pin < TOTAL_PINS) {
    switch(Firmata.getPinConfig(pin)) {
#ifdef SERVOFIRMATA
    case SERVO:
      if (IS_PIN_SERVO(pin)) {
        ServoFirmata.analogWrite(pin,value);
        Firmata.setPinState(pin,value);
      }
      break;
#endif
#ifdef ANALOGFIRMATA
    case PWM:
      if (IS_PIN_PWM(pin)) {
        analogWrite(PIN_TO_PWM(pin), value);
        Firmata.setPinState(pin,value);
      }
      break;
#endif
    }
  }
}

#endif
