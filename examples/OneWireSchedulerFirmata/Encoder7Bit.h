#ifndef Encoder7Bit_h
#define Encoder7Bit_h
#include "Arduino.h"
#include "FirmataExt.h"

#define num7BitOutbytes(a)(((a)*7)>>3)

class Encoder7BitClass {
public:
  Encoder7BitClass();
  void startBinaryWrite();
  void endBinaryWrite();
  void writeBinary(byte data);
  void writeBinary(int numBytes, byte *data);
  int calculateOutbytes(int inBytes);
  void readBinary(int outBytes,byte *inData, byte *outData);
  void sendValueAsTwo7bitBytes(int value); //this is private in Firmata
  int getValueFromTwo7bitBytes(byte *argv);

private:
  byte previous;
  int shift;
};

extern Encoder7BitClass Encoder7Bit;

#endif


