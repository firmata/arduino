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

#include <Firmata.h>

#include <utility/DigitalInputFirmata.h>

#include <utility/DigitalOutputFirmata.h>

#include <utility/AnalogInputFirmata.h>

#include <utility/AnalogOutputFirmata.h>

#include <Servo.h> //wouldn't load from ServoFirmata.h in Arduino1.0.3
#include <utility/ServoFirmata.h>

#if defined AnalogOutputFirmata_h || defined ServoFirmata_h
#include <utility/AnalogWrite.h>
#endif

#include <Wire.h> //wouldn't load from I2CFirmata.h in Arduino1.0.3
#include <utility/I2CFirmata.h>

#include <utility/OneWireFirmata.h>

#include <utility/StepperFirmata.h>

#include <utility/FirmataExt.h>

#include <utility/FirmataScheduler.h>

#if defined AnalogInputFirmata_h || defined DigitalInputFirmata || defined I2CFirmata_h
#include <utility/FirmataReporting.h>
#endif

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void systemResetCallback()
{
  // initialize a defalt state
  // TODO: option to load config from EEPROM instead of default

  // pins with analog capability default to analog input
  // otherwise, pins default to digital output
  for (byte i=0; i < TOTAL_PINS; i++) {
    if (IS_PIN_ANALOG(i)) {
#ifdef AnalogInputFirmata_h
      // turns off pullup, configures everything
      Firmata.setPinMode(i, ANALOG);
#endif
    } else {
#ifdef DigitalOutputFirmata_h
      // sets the output to 0, configures portConfigInputs
      Firmata.setPinMode(i, OUTPUT);
#endif
    }
  }

#ifdef FirmataExt_h
  FirmataExt.reset();
#endif
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void setup() 
{
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);

#if defined AnalogOutputFirmata_h || defined ServoFirmata_h
  /* analogWriteCallback is declared in AnalogWrite.h */
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
#endif

  #ifdef FirmataExt_h
#ifdef DigitalInputFirmata_h
  FirmataExt.addFeature(DigitalInputFirmata);
#endif
#ifdef DigitalOutputFirmata_h
  FirmataExt.addFeature(DigitalOutputFirmata);
#endif
#ifdef AnalogInputFirmata_h
  FirmataExt.addFeature(AnalogInputFirmata);
#endif
#ifdef AnalogOutputFirmata_h
  FirmataExt.addFeature(AnalogOutputFirmata);
#endif
#ifdef ServoFirmata_h
  FirmataExt.addFeature(ServoFirmata);
#endif
#ifdef I2CFirmata_h
  FirmataExt.addFeature(I2CFirmata);
#endif
#ifdef OneWireFirmata_h
  FirmataExt.addFeature(OneWireFirmata);
#endif
#ifdef StepperFirmata_h
  FirmataExt.addFeature(StepperFirmata);
#endif
#ifdef FirmataReporting_h
  FirmataExt.addFeature(FirmataReporting);
#endif
#ifdef FirmataScheduler_h
  FirmataExt.addFeature(FirmataScheduler);
#endif
#endif
  /* systemResetCallback is declared here (in ConfigurableFirmata.ino) */
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() 
{
#ifdef DigitalInputFirmata_h
  /* DIGITALREAD - as fast as possible, check for changes and output them to the
   * FTDI buffer using Serial.print()  */
  DigitalInputFirmata.report();
#endif

  /* SERIALREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while(Firmata.available()) {
    Firmata.processInput();
#ifdef FirmataScheduler_h
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

#ifdef FirmataReporting_h
  if (FirmataReporting.elapsed()) {
#ifdef AnalogInputFirmata_h
    /* ANALOGREAD - do all analogReads() at the configured sampling interval */
    AnalogInputFirmata.report();
#endif
#ifdef I2CFirmata_h
    // report i2c data for all device with read continuous mode enabled
    I2CFirmata.report();
#endif
  }
#endif
#ifdef StepperFirmata_h
  StepperFirmata.update();
#endif
}
