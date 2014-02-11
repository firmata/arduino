/*
  EncoderFirmata.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2014 Nicolas Panel. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Provide encoder feature based on PJRC implementation.
  See http://www.pjrc.com/teensy/td_libs_Encoder.html for more informations
  
  EncoderFirmata handle instructions and is able to automatically report positions.
  See protocol details in "examples / SimpleEncoderFirmata / SimpleEncoderFirmata.ino"
    
*/

#ifndef EncoderFirmata_h
#define EncoderFirmata_h

#include <Firmata.h>
#include "FirmataFeature.h"

// This optional setting causes Encoder to use more optimized code
// safe iif 'attachInterrupt' is never used in the same time
//#define ENCODER_OPTIMIZE_INTERRUPTS // => not compiling
#include "Encoder.h"

#define MAX_ENCODERS                5 // arbitrary value, may need to adjust
#define ENCODER_ATTACH              (0x00)
#define ENCODER_REPORT_POSITION     (0x01)
#define ENCODER_REPORT_POSITIONS    (0x02)
#define ENCODER_RESET_POSITION      (0x03)
#define ENCODER_REPORT_AUTO         (0x04)
#define ENCODER_DETACH              (0x05)
#define ENCODER_DATA                (0x61) // TODO : Move to Firmata.h
#define ENCODER_MESSAGE             (0x80) // TODO : Move to Firmata.h

class EncoderFirmata:public FirmataFeature
{
public:
  EncoderFirmata();
  ~EncoderFirmata();
  // FirmataFeature implementation
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte *argv);
  void reset();
  
  // EncoderFirmata implementation
  void report();
  boolean isEncoderAttached(byte encoderNum); 
  void attachEncoder(byte encoderNum, byte pinANum, byte pinBNum);
  void detachEncoder(byte encoderNum);
  void reportPosition(byte encoderNum);
  void reportEncodersPositions();
  void resetEncoderPosition(byte encoderNum);
  void toggleAutoReport(bool report);
  bool isReportingEnabled();

private:
  Encoder *encoders[MAX_ENCODERS];
  volatile bool autoReport;
};

#endif