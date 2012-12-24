#ifndef FirmataExt_h
#define FirmataExt_h

#include "Arduino.h"
#include "Firmata.h"

class FirmataExtClass
{
  public:
  void attach(sysexCallbackFunction newFunction);

  void sendBinarySysex(byte command, byte numBytes, byte *data);
  void sysexCallback(byte command, byte argc, byte *argv);

  private:
  byte decodedBytes[((MAX_DATA_BYTES-1)*7)/8];
  sysexCallbackFunction decodedSysexCallback;

  byte readBinarySysex(byte numInBytes, byte *inData);
};

extern FirmataExtClass FirmataExt;

#endif

