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
  
  EncoderFirmata handle sysex instructions and is able to automatically report positions.
  
  Sysex queries : 
    1. Attach encoder
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_ATTACH             (0x00)
     * 3 encoder #                  ([0 - MAX_ENCODERS-1])
     * 4 pin A #                    (first pin) 
     * 5 pin B #                    (second pin)
     * 6 END_SYSEX                  (0xF7)
     *--------------------
    2. Report encoder position
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_REPORT_POSITION    (0x01)
     * 3 encoder #                  ([0 - MAX_ENCODERS-1])
     * 4 END_SYSEX                  (0xF7)
     *--------------------
    3. Report encoders positions
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_REPORT_POSITIONS   (0x02)
     * 3 END_SYSEX                  (0xF7)
     *--------------------
    4. Reset encoder position to zero
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_RESET_POSITION     (0x03)
     * 3 encoder #                  ([0 - MAX_ENCODERS-1])
     * 4 END_SYSEX                  (0xF7)
     *--------------------
    5. Enable/disable reporting ()
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_REPORT_AUTO        (0x04)
     * 3 enable                     (0x00 => false, true otherwise)
     * 4 END_SYSEX                  (0xF7)
     *--------------------
    6. Detach encoder
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_DETACH             (0x05)
     * 3 encoder #                  ([0 - MAX_ENCODERS-1])
     * 4 END_SYSEX                  (0xF7)
     *--------------------
    
    
*/

#ifndef EncoderFirmata_h
#define EncoderFirmata_h

#include <Firmata.h>
#include "FirmataFeature.h"

// This optional setting causes Encoder to use more optimized code
// safe only if 'attachInterrupt' is never used in the same time
//#define ENCODER_OPTIMIZE_INTERRUPTS 
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
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte *argv);
  void reset();
  void report();
  boolean isEncoderAttached(byte encoderNum); 
  volatile bool autoReport;

private:
  void attachEncoder(byte encoderNum, byte pinANum, byte pinBNum);
  void detachEncoder(byte encoderNum);
  void reportEncoder(byte encoderNum);
  void reportEncodersPositions();
  void resetEncoderPosition(byte encoderNum);
  void toggleAutoReport(bool report);
  Encoder *encoders[MAX_ENCODERS];
};

#endif