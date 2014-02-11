/*
  EncoderFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2013 Nicolas Panel. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
  
  Provide encoder implementation based on Arduino external interrupts
  see http://arduino.cc/en/Reference/attachInterrupt for more informations
*/

#include <Firmata.h>
#include "EncoderFirmata.h"
// This optional setting causes Encoder to use more optimized code
// safe only if 'attachInterrupt' is never used in the same time
#define ENCODER_OPTIMIZE_INTERRUPTS 
#include "Encoder.h"

/* Constructor */
EncoderFirmata::EncoderFirmata()
{
  byte encoder;
  for(encoder=0; encoder<MAX_ENCODERS; encoder++) 
  {
    encoders[encoder]=NULL;
  }
  autoReport = false;
}
EncoderFirmata::~EncoderFirmata()
{
  delete[] encoders;
}

void EncoderFirmata::attachEncoder(byte encoderNum, byte pinANum, byte pinBNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    Firmata.sendString("Encoder Warning: encoder is already attached. Operation cancelled.");
    return;
  }
  
  if (!IS_PIN_INTERRUPT(pinANum) || !IS_PIN_INTERRUPT(pinBNum))
  {
    Firmata.sendString("Encoder Warning: For better performences, you should only use Interrput pins." );
  }
  encoders[encoderNum] = new Encoder(pinANum, pinBNum);
}

void EncoderFirmata::detachEncoder(byte encoderNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    delete encoders[encoderNum];
    encoders[encoderNum] = NULL;
  }
}

boolean EncoderFirmata::handlePinMode(byte pin, int mode)
{
  if (IS_PIN_INTERRUPT(pin)) 
  {
    // nothing to do, pins states are managed 
    // in "attach/detachEncoder" methods
    return true;
  }
  return false;
}

void EncoderFirmata::handleCapability(byte pin)
{
  // nothing to do
}


/* Handle ENCODER_DATA (0x61) sysex commands
1. Attach encoder
   * ------------------
   * 0 ENCODER_ATTACH             (0x00)
   * 1 encoder #                  ([0 - MAX_ENCODERS-1])
   * 2 pin A #                    (first pin) 
   * 3 pin B #                    (second pin)
   *--------------------
  2. Report encoder position
   * ------------------
   * 0 ENCODER_REPORT_POSITION    (0x01)
   * 1 encoder #                  ([0 - MAX_ENCODERS-1])
   *--------------------
  3. Report encoders positions
   * ------------------
   * 0 ENCODER_REPORT_POSITIONS   (0x02)
   *--------------------
  4. Reset encoder position to zero
   * ------------------
   * 0 ENCODER_RESET_POSITION     (0x03)
   * 1 encoder #                  ([0 - MAX_ENCODERS-1])
   *--------------------
  5. Enable/disable reporting ()
   * ------------------
   * 0 ENCODER_REPORT_AUTO        (0x04)
   * 1 enable                     (0x00 => false, true otherwise)
   *--------------------
  6. Detach encoder
   * ------------------
   * 0 ENCODER_DETACH             (0x05)
   * 1 encoder #                  ([0 - MAX_ENCODERS-1])
   *--------------------
*/
boolean EncoderFirmata::handleSysex(byte command, byte argc, byte *argv)
{
  if (command == ENCODER_DATA) 
  {
    byte encoderCommand, encoderNum, pinA, pinB, enableReports; 
    
    encoderCommand= argv[0];

    if (encoderCommand == ENCODER_ATTACH) 
    {
      encoderNum = argv[1];
      pinA = argv[2];
      pinB = argv[3];
      attachEncoder(encoderNum, pinA, pinB);
      return true;
    }
    
    
    if (encoderCommand == ENCODER_REPORT_POSITION)
    {
      encoderNum = argv[1];
      reportEncoder(encoderNum);
      return true;
    }
    
    if (encoderCommand == ENCODER_REPORT_POSITIONS)
    {
      reportEncodersPositions();
      return true;
    }
    
    if (encoderCommand == ENCODER_RESET_POSITION)
    {
      encoderNum = argv[1];
      resetEncoderPosition(encoderNum);
      return true;
    }
    if (encoderCommand == ENCODER_REPORT_AUTO)
    {
      enableReports = argv[1];
      toggleAutoReport(enableReports == 0x00 ? false : true);
      return true;
    }
    
    if (encoderCommand == ENCODER_DETACH)
    {
      encoderNum = argv[1];
      detachEncoder(encoderNum);
      return true;
    }
    
    Firmata.sendString("Encoder Error: Invalid command");
  }
  return false;
}

void EncoderFirmata::reset()
{
  byte encoder;
  for(encoder=0; encoder<MAX_ENCODERS; encoder++) 
  {
    detachEncoder(encoder);
  }
  autoReport = false;
}

void EncoderFirmata::report()
{
  if (autoReport)
  {
    reportEncodersPositions();
  }
}

boolean EncoderFirmata::isEncoderAttached(byte encoderNum)
{
  if (encoderNum>=MAX_ENCODERS)
  {
    Firmata.sendString("Encoder Error: encoder number should be less than 5. Operation cancelled.");
    return false;
  }
  if (encoders[encoderNum]) 
  {
    return true;
  }
  else
  {
    return false;
  }
}

void EncoderFirmata::resetEncoderPosition(byte encoderNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    encoders[encoderNum]->write(0);
  }
}

/* Report encoder's postion (one message per encoder) 
 * 
 * MIDI protocol
 *---------------------
 * 0 ENCODER_MESSAGE (0x80) | Channel (encoder #, 0-4)
 * 1 direction (positive = 0, negative = 1)
 * 2 current position, bits 0-6
 * 3 current position, bits 7-13
 * 4 current position, bits 14-20
 * 5 current position, bits 21-27
 *--------------------
*/
void EncoderFirmata::reportEncoder(byte encoderNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    signed long position = encoders[encoderNum]->read();
    long absValue = abs(position);
    Firmata.write(ENCODER_MESSAGE | (encoderNum & 0xF));
    Firmata.write(position >= 0 ? 0x00 : 0x01);
    Firmata.write((byte)absValue & 0x7F);
    Firmata.write((byte)(absValue >> 7) & 0x7F);
    Firmata.write((byte)(absValue >> 14) & 0x7F);
    Firmata.write((byte)(absValue >> 21) & 0x7F);
  }
}
/* Report encoders postions (one message for all encoders) 
 * 
 * Sysex message
 *---------------------
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
 *--------------------
*/
void EncoderFirmata::reportEncodersPositions()
{
  Firmata.write(START_SYSEX);
  Firmata.write(ENCODER_DATA);
  byte encoder;
  for(encoder=0; encoder<MAX_ENCODERS; encoder++) 
  {
    reportEncoder(encoder);
  }
  Firmata.write(END_SYSEX);
}


void EncoderFirmata::toggleAutoReport(bool report)
{
  autoReport = report;
}

bool EncoderFirmata::isReportingEnabled()
{
  return autoReport;
}

