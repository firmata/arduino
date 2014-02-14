/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please click on the following link
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
  Copyright (C) 2014 Nicolas Panel. All rights reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 
  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
 
 * =====================================================
 *  Encoder PROTOCOL
 * =====================================================
 *
 *  Attach encoder query
 * -----------------------------------------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_ATTACH             (0x00)
 * 3 encoder #                  ([0 - MAX_ENCODERS-1])
 * 4 pin A #                    (first pin) 
 * 5 pin B #                    (second pin)
 * 6 END_SYSEX                  (0xF7)
 * -----------------------------------------------------
 *  
 *  Report encoder's position
 * -----------------------------------------------------
 * Sysex Query
 * ------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_REPORT_POSITION    (0x01)
 * 3 encoder #                  ([0 - MAX_ENCODERS-1])
 * 4 END_SYSEX                  (0xF7)
 *--------------------
 * MIDI Response 
 * ------------------
 * 0 ENCODER_MESSAGE (0x80) | Channel (encoder #, 0-4)
 * 1 direction (positive = 0, negative = 1)
 * 2 current position, bits 0-6
 * 3 current position, bits 7-13
 * 4 current position, bits 14-20
 * 5 current position, bits 21-27
 * -----------------------------------------------------
 * 
 * Report all encoders positions
 * -----------------------------------------------------
 * Sysex Query
 * ------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_REPORT_POSITIONS   (0x02)
 * 3 END_SYSEX                  (0xF7)
 *--------------------
 * Sysex Response
 * ------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 first encoder #            ([0 - MAX_ENCODERS-1])
 * 3 first enc. dir.            (positive = 0, negative = 1)
 * 4 first enc. position, bits 0-6
 * 5 first enc. position, bits 7-13
 * 6 first enc. position, bits 14-20
 * 7 first enc. position, bits 21-27
 * ...
 * N END_SYSEX                  (0xF7)
 * -----------------------------------------------------
 *
 * Reset encoder position to zero (Sysex Query)
 * -----------------------------------------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_RESET_POSITION     (0x03)
 * 3 encoder #                  ([0 - MAX_ENCODERS-1])
 * 4 END_SYSEX                  (0xF7)
 * -----------------------------------------------------
 * 
 * Enable/disable reporting (Sysex Query)
 * -----------------------------------------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_REPORT_AUTO        (0x04)
 * 3 enable                     (0x00 => false, true otherwise)
 * 4 END_SYSEX                  (0xF7)
 * -----------------------------------------------------
 *
 * Detach encoder (Sysex Query)
 * -----------------------------------------------------
 * 0 START_SYSEX                (0xF0)
 * 1 ENCODER_DATA               (0x61)
 * 2 ENCODER_DETACH             (0x05)
 * 3 encoder #                  ([0 - MAX_ENCODERS-1])
 * 4 END_SYSEX                  (0xF7)
 *--------------------
 */

//* Due to arduino issue, Wire and Servo libraries need to be included. Should be fixed in later versions
#include <Wire.h>
#include <Servo.h>
//*/
#include <Firmata.h>
#include <utility/FirmataExt.h>
#include <utility/FirmataReporting.h>
#include <utility/EncoderFirmata.h>

FirmataExt firmataExt;
FirmataReporting reporting;
EncoderFirmata encoder;

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/
void systemResetCallback()
{
  firmataExt.reset();
}

/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup() 
{
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  firmataExt.addFeature(reporting);
  firmataExt.addFeature(encoder);
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
  /* STREAMREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while(Firmata.available()) {
    Firmata.processInput();
  }

  if (reporting.elapsed()) {
    encoder.report();
  }
}
