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
#include <string.h>

#define isAttached(encoderNum) (encoderNum < MAX_ENCODERS && encoders[encoderNum])

static Encoder *encoders[MAX_ENCODERS];
static int32_t positions[MAX_ENCODERS];
static byte autoReport = 0x02;

/* Constructor */
EncoderFirmata::EncoderFirmata()
{
  memset(encoders,0,sizeof(Encoder*)*MAX_ENCODERS);
}

void EncoderFirmata::attachEncoder(byte encoderNum, byte pinANum, byte pinBNum)
{
  if (isAttached(encoderNum))
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
  reportPosition(encoderNum);
}

void EncoderFirmata::detachEncoder(byte encoderNum)
{
  if (isAttached(encoderNum))
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
      autoReport = argv[1];
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
  autoReport = 0x02;
}

void EncoderFirmata::report()
{
  if (autoReport > 0)
  {
    bool report = false;
    for (uint8_t encoderNum=0; encoderNum < MAX_ENCODERS; encoderNum++)
    {
      if (isAttached(encoderNum))
      {
        int32_t position = encoders[encoderNum]->read();
        if ( autoReport == 1 || positions[encoderNum] != position )
        {
          if (!report)
          {
            Firmata.write(START_SYSEX);
            Firmata.write(ENCODER_DATA);
            report = true;
          }
          positions[encoderNum] = position;
          _reportEncoderPosition(encoderNum,position);
        }
      }
    }
    if (report)
    {
      Firmata.write(END_SYSEX);
    }
  }
}

boolean EncoderFirmata::isEncoderAttached(byte encoderNum)
{
  return isAttached(encoderNum);
}

void EncoderFirmata::resetPosition(byte encoderNum)
{
  if (isAttached(encoderNum))
  {
    encoders[encoderNum]->write(0);
  }
}

// Report specify encoder postion using midi protocol
void EncoderFirmata::reportPosition(byte encoder)
{
  if (isAttached(encoder))
  {
    Firmata.write(START_SYSEX);
    Firmata.write(ENCODER_DATA);

    _reportEncoderPosition(encoder,encoders[encoder]->read());

    Firmata.write(END_SYSEX);
  }
}
// Report all attached encoders positions (one message for all encoders) 
void EncoderFirmata::reportPositions()
{
  byte tmpReport = autoReport;
  autoReport = 1;
  report();
  autoReport = tmpReport;
}

void EncoderFirmata::_reportEncoderPosition(byte encoder, int32_t position)
{
  long absValue = abs(position);
  byte direction = position >= 0 ? 0x00 : 0x01;
  Firmata.write((direction << 6) | (encoder));
  Firmata.write((byte)absValue & 0x7F);
  Firmata.write((byte)(absValue >> 7) & 0x7F);
  Firmata.write((byte)(absValue >> 14) & 0x7F);
  Firmata.write((byte)(absValue >> 21) & 0x7F);
}

void EncoderFirmata::toggleAutoReport(byte report)
{
  autoReport = report;
}

bool EncoderFirmata::isReportingEnabled()
{
  return autoReport > 0;
}


