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
  
  Provide encoder feature based on PJRC implementation.
  See http://www.pjrc.com/teensy/td_libs_Encoder.html for more informations
*/

#include <Firmata.h>
#include "EncoderFirmata.h"
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

void EncoderFirmata::attachEncoder(byte encoderNum, byte pinANum, byte pinBNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    //Firmata.sendString("Encoder Warning: encoder is already attached. Operation cancelled.");
    return;
  }
  
  if (!IS_PIN_INTERRUPT(pinANum) || !IS_PIN_INTERRUPT(pinBNum))
  {
    //Firmata.sendString("Encoder Warning: For better performences, you should only use Interrput pins.");
  }
  Firmata.setPinMode(pinANum, ENCODER);
  Firmata.setPinMode(pinBNum, ENCODER);
  encoders[encoderNum] = new Encoder(pinANum, pinBNum);
}

void EncoderFirmata::detachEncoder(byte encoderNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    free(encoders[encoderNum]);
    encoders[encoderNum] = NULL;
  }
}

boolean EncoderFirmata::handlePinMode(byte pin, int mode)
{
  if (mode == ENCODER) {
    if (IS_PIN_INTERRUPT(pin)) 
    {
      // nothing to do, pins states are managed 
      // in "attach/detach Encoder" methods
      return true;
    }
  }
  return false;
}

void EncoderFirmata::handleCapability(byte pin)
{
  if (IS_PIN_INTERRUPT(pin)) {
    Firmata.write(ENCODER);
    Firmata.write(28); //28 bits used for absolute position
  }
}


/* Handle ENCODER_DATA (0x61) sysex commands
 * See protocol details in "examples/SimpleEncoderFirmata/SimpleEncoderFirmata.ino"
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
      if (Firmata.getPinMode(pinA)==IGNORE || Firmata.getPinMode(pinB)==IGNORE)
      {
        return false;
      }  
      attachEncoder(encoderNum, pinA, pinB);
      return true;
    }
    
    
    if (encoderCommand == ENCODER_REPORT_POSITION)
    {
      encoderNum = argv[1];
      reportPosition(encoderNum);
      return true;
    }
    
    if (encoderCommand == ENCODER_REPORT_POSITIONS)
    {
      reportPositions();
      return true;
    }
    
    if (encoderCommand == ENCODER_RESET_POSITION)
    {
      encoderNum = argv[1];
      resetPosition(encoderNum);
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
    
    //Firmata.sendString("Encoder Error: Invalid command");
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
    byte encoder;
    for(encoder=0; encoder<MAX_ENCODERS; encoder++) 
    {
      reportPosition(encoder);
    }
  }
}

boolean EncoderFirmata::isEncoderAttached(byte encoderNum)
{
  if (encoderNum>=MAX_ENCODERS)
  {
    //Firmata.sendString("Encoder Error: encoder number should be less than 5. Operation cancelled.");
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

void EncoderFirmata::resetPosition(byte encoderNum)
{
  if (isEncoderAttached(encoderNum)) 
  {
    encoders[encoderNum]->write(0);
  }
}

// Report specify encoder postion using midi protocol
void EncoderFirmata::reportPosition(byte encoder)
{
  if (isEncoderAttached(encoder)) 
  {
    Firmata.write(START_SYSEX);
    Firmata.write(ENCODER_DATA);

    _reportEncoderPosition(encoder);

    Firmata.write(END_SYSEX);
  }
}
// Report all attached encoders positions (one message for all encoders) 
void EncoderFirmata::reportPositions()
{
  Firmata.write(START_SYSEX);
  Firmata.write(ENCODER_DATA);
  byte encoder;
  for(encoder=0; encoder<MAX_ENCODERS; encoder++) 
  {
    _reportEncoderPosition(encoder);
  }
  Firmata.write(END_SYSEX);
}
void EncoderFirmata::_reportEncoderPosition(byte encoder)
{
  if (isEncoderAttached(encoder)) 
  {
    signed long position = encoders[encoder]->read();
    long absValue = abs(position);
    byte direction = position >= 0 ? 0x00 : 0x01;
    Firmata.write((direction << 7) | (encoder));
    Firmata.write((byte)absValue & 0x7F);
    Firmata.write((byte)(absValue >> 7) & 0x7F);
    Firmata.write((byte)(absValue >> 14) & 0x7F);
    Firmata.write((byte)(absValue >> 21) & 0x7F);
  }
}


void EncoderFirmata::toggleAutoReport(bool report)
{
  autoReport = report;
}

bool EncoderFirmata::isReportingEnabled()
{
  return autoReport;
}

