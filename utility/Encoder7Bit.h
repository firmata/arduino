/*
  Encoder7Bit.h - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef Encoder7Bit_h
#define Encoder7Bit_h
#include <Arduino.h>

#define num7BitOutbytes(a)(((a)*7)>>3)

class Encoder7BitClass
{
public:
  Encoder7BitClass();
  void startBinaryWrite();
  void endBinaryWrite();
  void writeBinary(byte data);
  void readBinary(int outBytes,byte *inData, byte *outData);

private:
  byte previous;
  int shift;
};

extern Encoder7BitClass Encoder7Bit;

#endif


