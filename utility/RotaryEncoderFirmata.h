/*
  RotaryEncoderFirmata.h - Firmata library
  Copyright (C) 2013 Alan Yorinks. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef Rotary_Encoder_Firmata_h
#define Rotary_Encoder_Firmata_h


#include <Firmata.h>
#include <utility/FirmataFeature.h>
#include  <utility/RotaryEncoder.h>




#define ROTARY_ENCODER_NOT_PRESENT  0
#define ROTARY_ENCODER_IS_PRESENT   1

class RotaryEncoderFirmata:public FirmataFeature
{
public:
  RotaryEncoderFirmata() ;
  void report() ;
  boolean handleSysex(byte command, byte argc, byte *argv);
  void handleCapability(byte pin) ;
  boolean handlePinMode(byte pin, int mode);
  void reset();

private:
  byte  encoderMSB, encoderLSB ;     // sysex data registers
  uint8_t encoderPin1, encoderPin2 ; // user specified encoder pins
  int encoderPosition ;              // current position of encoder
  int8_t clicks ;                    // encoder click counter
  boolean encoderPresent ;           // encoder installed flag
  RotaryEncoder *theEncoder ;           // pointer to encoder object   
}; 

#endif
