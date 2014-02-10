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

  Usage : 
  EncoderFirmata manages the following sysex instructions : 
    1. Attach encoder
     * ------------------
     * 0 START_SYSEX                (0xF0)
     * 1 ENCODER_DATA               (0x61)
     * 2 ENCODER_ATTACH             (0x00)
     * 3 encoder #                  ([0 - MAX_ENCODERS-1])
     * 4 pin1 #                     (first pin) 
     * 5 pin2 #                     (second pin)
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
    
    It is also able to automatically report positions
*/

#ifndef EncoderFirmata_h
#define EncoderFirmata_h

#include <Firmata.h>
#include <utility/FirmataFeature.h>
#include <FirmataReporting.h>

#define MAX_ENCODERS                5 // arbitrary value, may need to adjust
#define ENCODER_ATTACH              0x00
#define ENCODER_REPORT_POSITION     0x01
#define ENCODER_REPORT_POSITIONS    0x02
#define ENCODER_RESET_POSITION      0x03
#define ENCODER_REPORT_AUTO         0x04
#define ENCODER_DETACH              0x05

class EncoderFirmata:public FirmataFeature
{
public:
  EncoderFirmata();
  boolean handlePinMode(byte pin, int mode);
  void handleCapability(byte pin);
  boolean handleSysex(byte command, byte argc, byte *argv);
  void reset();
  void report();
  volatile signed long encoderPositions[MAX_ENCODERS];
  byte pins[MAX_ENCODERS][2]; // [ pinA index, pinB index]
  volatile byte pinStates[MAX_ENCODERS][2]; // [ pinA state, pinB state ]
  boolean isEncoderAttached(byte encoderNum);

private:
  void attachEncoder(byte encoderNum, byte pin1Num, byte pin2Num);
  void detachEncoder(byte encoderNum);
  void reportEncoder(byte encoderNum);
  void toggleAutoReport(bool report)
  bool _isEncoderAttached[MAX_ENCODERS]; 
  byte numEncoders;
  bool auto_report;
};


#endif