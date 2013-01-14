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
 Copyright (C) 2009 Jeff Hoefs.  All rights reserved.
 Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 See file LICENSE.txt for further informations on licensing terms.
 */

#include <OneWire.h> //this is included here because it would'nt load from OneWireFirmata.h
#include "Firmata.h"
#include "FirmataExt.h"
#include "FirmataScheduler.h"
#include "OneWireFirmata.h"

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* pins configuration */
byte pinConfig[TOTAL_PINS];         // configuration of every pin

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/

void sysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case ONEWIRE_REQUEST: 
    {
      if (argc>0) {
        OneWireFirmata.handleOneWireRequest(argv[0],argc-1,argv+1);
      }
      break;
    }
  case SCHEDULER_REQUEST: 
    {
      if (argc>0) {
        FirmataScheduler.handleSchedulerRequest(argv[0],argc-1,argv+1);
      }
      break;
    }
  }
}

void setPinModeCallback(byte pin, int mode)
{
  switch(mode) {
  case ONEWIRE:
    pinConfig[pin] = ONEWIRE;
    OneWireFirmata.oneWireConfig(pin,ONEWIRE_POWER);
    break;
  default:
    Firmata.sendString("Invalid pinMode");
  }
}

void systemResetCallback() {
  for (int i=0;i<TOTAL_PINS;i++) {
    pinConfig[i] = 0;
  }
  OneWireFirmata.reset();
  FirmataScheduler.reset();
}

void setup()
{
  Firmata.setFirmwareVersion(2, 4);

  Firmata.attach(SET_PIN_MODE, setPinModeCallback);
  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
}

void loop()
{
  Firmata.processInput();
  if (!Firmata.isParsingMessage()) {
    FirmataScheduler.runTasks();
  }
}








