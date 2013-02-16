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

void Encoder7BitClass::writeBinary(int numBytes, byte *data)
{
  int messageSize = (numBytes << 3) / 7;
  if (((numBytes << 3) % 7) > 0) {
    messageSize++;
  }
  for (int i = 0; i < messageSize; i++) {
    int j = i * 7;
    int pos = j >> 3;   // 0,0,1,2,3,4,5,6,7,7,8...
    byte shift = j & 7;  // 0,7,6,5,4,3,2,1,0,7,6...
    byte out = (data[pos] >> shift) & 0x7F;
    if (shift > 1 && pos < (numBytes - 1)) {
      out |= (data[pos + 1] << (8 - shift)) & 0x7F;
    }
    Firmata.write(out);
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

void Encoder7BitClass::sendValueAsTwo7bitBytes(int value) // is private in Firmata...
{
  Firmata.write(value & B01111111); // LSB
  Firmata.write(value >> 7 & B01111111); // MSB
};

int Encoder7BitClass::getValueFromTwo7bitBytes(byte *argv) {
  return (argv[0] & B01111111) | (argv[1] << 7);
};

Encoder7BitClass Encoder7Bit;

