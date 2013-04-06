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
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 
  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/


#include <Firmata.h>

// To configure, save this file to your working directory so you can edit it
// then comment out the include and declaration for any features that you do 
// not need on lines 41 - 71.

// Also note that the current compile size for an Arduino Uno with all of the
// following features enabled is about 22.4k. If you are using an older Arduino
// or other microcontroller with less memory you will not be able to include
// all of the following feature classes.

#include <utility/DigitalInputFirmata.h>
DigitalInputFirmata digitalInput;

#include <utility/DigitalOutputFirmata.h>
DigitalOutputFirmata digitalOutput;

#include <utility/AnalogInputFirmata.h>
AnalogInputFirmata analogInput;

#include <utility/AnalogOutputFirmata.h>
AnalogOutputFirmata analogOutput;

#include <Servo.h> //wouldn't load from ServoFirmata.h in Arduino1.0.3
#include <utility/ServoFirmata.h>
ServoFirmata servo;

#include <Wire.h> //wouldn't load from I2CFirmata.h in Arduino1.0.3
#include <utility/I2CFirmata.h>
I2CFirmata i2c;

#include <utility/OneWireFirmata.h>
OneWireFirmata oneWire;

#include <utility/StepperFirmata.h>
StepperFirmata stepper;

#include <utility/FirmataExt.h>
FirmataExt firmataExt;

#include <utility/FirmataScheduler.h>
FirmataScheduler scheduler;


// dependencies. Do not comment out the following lines
#if defined AnalogOutputFirmata_h || defined ServoFirmata_h
#include <utility/AnalogWrite.h>
#endif

#if defined AnalogInputFirmata_h || defined I2CFirmata_h
#include <utility/FirmataReporting.h>
FirmataReporting reporting;
#endif

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void systemResetCallback()
{
  // initialize a defalt state

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
  firmataExt.reset();
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
  firmataExt.addFeature(digitalInput);
#endif
#ifdef DigitalOutputFirmata_h
  firmataExt.addFeature(digitalOutput);
#endif
#ifdef AnalogInputFirmata_h
  firmataExt.addFeature(analogInput);
#endif
#ifdef AnalogOutputFirmata_h
  firmataExt.addFeature(analogOutput);
#endif
#ifdef ServoFirmata_h
  firmataExt.addFeature(servo);
#endif
#ifdef I2CFirmata_h
  firmataExt.addFeature(i2c);
#endif
#ifdef OneWireFirmata_h
  firmataExt.addFeature(oneWire);
#endif
#ifdef StepperFirmata_h
  firmataExt.addFeature(stepper);
#endif
#ifdef FirmataReporting_h
  firmataExt.addFeature(reporting);
#endif
#ifdef FirmataScheduler_h
  firmataExt.addFeature(scheduler);
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
   * stream buffer using Firmata.write()  */
  digitalInput.report();
#endif

  /* STREAMREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while(Firmata.available()) {
    Firmata.processInput();
#ifdef FirmataScheduler_h
    if (!Firmata.isParsingMessage()) {
      goto runtasks;
    }
  }
  if (!Firmata.isParsingMessage()) {
runtasks: scheduler.runTasks();
#endif
  }

  /* SEND STREAM WRITE BUFFER - TO DO: make sure that the stream buffer doesn't go over
   * 60 bytes. use a timer to sending an event character every 4 ms to
   * trigger the buffer to dump. */

#ifdef FirmataReporting_h
  if (reporting.elapsed()) {
#ifdef AnalogInputFirmata_h
    /* ANALOGREAD - do all analogReads() at the configured sampling interval */
    analogInput.report();
#endif
#ifdef I2CFirmata_h
    // report i2c data for all device with read continuous mode enabled
    i2c.report();
#endif
  }
#endif
#ifdef StepperFirmata_h
  stepper.update();
#endif
}
