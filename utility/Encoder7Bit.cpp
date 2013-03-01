/*
  Encoder7Bit.cpp - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "Encoder7Bit.h"
#include <Firmata.h>

Encoder7BitClass::Encoder7BitClass()
{
  previous = 0;
  shift = 0;
}

void Encoder7BitClass::startBinaryWrite()
{
  shift = 0;
}

void Encoder7BitClass::endBinaryWrite()
{
  if (shift > 0) {
    Firmata.write(previous);
  }
}

void Encoder7BitClass::writeBinary(byte data)
{
  if (shift == 0) {
    Firmata.write(data & 0x7f);
    shift++;
    previous = data >> 7;
  } 
  else {
    Firmata.write(((data << shift) & 0x7f) | previous);
    if (shift == 6) {
      Firmata.write(data >> 1);
      shift = 0;
    } 
    else {
      shift++;
      previous = data >> (8 - shift);
    }
  }
}

void Encoder7BitClass::readBinary(int outBytes,byte *inData, byte *outData)
{
  for (int i=0;i<outBytes;i++) {
    int j=i<<3;
    int pos=j/7;
    byte shift=j%7;
    outData[i]=(inData[pos]>>shift)|((inData[pos+1]<<(7-shift))&0xFF);
  }
}

Encoder7BitClass Encoder7Bit;

