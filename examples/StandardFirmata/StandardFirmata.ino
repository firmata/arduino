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
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 
  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/

/* 
 * TODO: use Program Control to load stored profiles from EEPROM
 */

#include "FirmataConfig.h"
#include <Firmata.h>

#ifdef DIGITALFIRMATA
#include "DigitalFirmata.h"
#endif
#ifdef ANALOGFIRMATA
#include "AnalogFirmata.h"
#endif
#ifdef SERVOFIRMATA
#include <Servo.h> //wouldn't load from ServoFirmata.h in Arduino1.0.3
#include "ServoFirmata.h"
#endif
#if defined ANALOGFIRMATA || defined SERVOFIRMATA
#include "AnalogWrite.h"
#endif
#ifdef I2CFIRMATA
#include <Wire.h> //wouldn't load from I2CFirmata.h in Arduino1.0.3
#include "I2CFirmata.h"
#endif
#ifdef ONEWIREFIRMATA
#include "OneWireFirmata.h"
#endif

#ifdef FIRMATAEXT
#include "FirmataExt.h"
#endif
#ifdef FIRMATAREPORTING
#include "FirmataReporting.h"
#endif
#ifdef FIRMATASCHEDULER
#include "FirmataScheduler.h"
#endif

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

// -----------------------------------------------------------------------------
/* sets the pin mode to the correct state and sets the relevant bits in the
 * two bit-arrays that track Digital I/O and PWM status
 */
void setPinModeCallback(byte pin, int mode)
{
  boolean known = false;
#ifdef DIGITALFIRMATA
  known |= DigitalFirmata.handlePinMode(pin,mode);
#endif
#ifdef ANALOGFIRMATA
  known |= AnalogFirmata.handlePinMode(pin,mode);
#endif
#ifdef SERVOFIRMATA
  known |= ServoFirmata.handlePinMode(pin,mode);
#endif
#ifdef I2CFIRMATA
  known |= I2CFirmata.handlePinMode(pin,mode);
#endif
#ifdef ONEWIREFIRMATA
  known |= OneWireFirmata.handlePinMode(pin,mode);
#endif
  if (!known) {
    Firmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
  }
  // TODO: save status to EEPROM here, if changed
}

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/

#ifdef FIRMATAEXT
void capabilityQueryCallback(byte pin)
{
#ifdef DIGITALFIRMATA
  DigitalFirmata.handleCapability(pin);
#endif
#ifdef ANALOGFIRMATA
  AnalogFirmata.handleCapability(pin);
#endif
#ifdef SERVOFIRMATA
  ServoFirmata.handleCapability(pin);
#endif
#ifdef I2CFIRMATA
  I2CFirmata.handleCapability(pin);
#endif
#ifdef ONEWIREFIRMATA
  OneWireFirmata.handleCapability(pin);
#endif
}
#endif

void sysexCallback(byte command, byte argc, byte *argv)
{
#ifdef ANALOGFIRMATA
  if (AnalogFirmata.handleSysex(command,argc,argv)) return;
#endif
#ifdef SERVOFIRMATA
  if (ServoFirmata.handleSysex(command,argc,argv)) return;
#endif
#ifdef I2CFIRMATA
  if (I2CFirmata.handleSysex(command,argc,argv)) return;
#endif
#ifdef ONEWIREFIRMATA
  if (OneWireFirmata.handleSysex(command,argc,argv)) return;
#endif
#ifdef FIRMATASCHEDULER
  if (FirmataScheduler.handleSysex(command,argc,argv)) return;
#endif
#ifdef FIRMATAEXT
  if (FirmataExt.handleSysex(command,argc,argv)) return;
#endif
#ifdef FIRMATAREPORTING
  if (FirmataReporting.handleSysex(command,argc,argv)) return;
#endif  
  Firmata.sendString("Unknown sysex command");
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void systemResetCallback()
{
  // initialize a defalt state
  // TODO: option to load config from EEPROM instead of default
#ifdef I2CFIRMATA
  I2CFirmata.reset();
#endif
#ifdef DIGITALFIRMATA
  DigitalFirmata.reset();
#endif
#ifdef ANALOGFIRMATA
  AnalogFirmata.reset();
#endif
#ifdef ONEWIREFIRMATA
  OneWireFirmata.reset();
#endif
#ifdef FIRMATASCHEDULER
  FirmataScheduler.reset();
#endif
}

void setup() 
{
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);

#ifdef DIGITALFIRMATA
  Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
#ifdef FIRMATAREPORTING
  Firmata.attach(REPORT_DIGITAL, reportDigitalCallback);
#endif
#endif
#if defined ANALOGFIRMATA || defined SERVOFIRMATA
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
#endif
#if defined ANALOGFIRMATA && defined FIRMATAREPORTING
  Firmata.attach(REPORT_ANALOG, reportAnalogCallback);
#endif
  Firmata.attach(SET_PIN_MODE, setPinModeCallback);
#if defined ANALOGFIRMATA || defined SERVOFIRMATA || defined I2CFIRMATA || defined FIRMATAEXT || defined FIRMATAREPORTING
  Firmata.attach(START_SYSEX, sysexCallback);
#endif
#ifdef FIRMATAEXT
  FirmataExt.attach(capabilityQueryCallback);
#endif
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() 
{
#ifdef DIGITALFIRMATA
  /* DIGITALREAD - as fast as possible, check for changes and output them to the
   * FTDI buffer using Serial.print()  */
  DigitalFirmata.checkDigitalInputs();
#endif

  /* SERIALREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while(Firmata.available()) {
    Firmata.processInput();
#ifdef FIRMATASCHEDULER
    if (!Firmata.isParsingMessage()) {
      goto runtasks;
    }
  }
  if (!Firmata.isParsingMessage()) {
runtasks: FirmataScheduler.runTasks();
#endif
  }

  /* SEND FTDI WRITE BUFFER - make sure that the FTDI buffer doesn't go over
   * 60 bytes. use a timer to sending an event character every 4 ms to
   * trigger the buffer to dump. */

#ifdef FIRMATAREPORTING
  if (FirmataReporting.elapsed()) {
#ifdef ANALOGFIRMATA
    /* ANALOGREAD - do all analogReads() at the configured sampling interval */
    AnalogFirmata.report();
#endif
#ifdef I2CFIRMATA    
    // report i2c data for all device with read continuous mode enabled
    I2CFirmata.report();
#endif
  }
#endif
}
