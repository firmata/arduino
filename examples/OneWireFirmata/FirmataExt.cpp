#include "Arduino.h"
#include "Firmata.h"
#include "FirmataExt.h"

void FirmataExtClass::attach(sysexCallbackFunction newFunction) {
  decodedSysexCallback = newFunction;
}

void FirmataExtClass::sysexCallback(byte command, byte argc, byte *argv) {
  if (decodedSysexCallback) {
    byte numBytes = readBinarySysex(argc,argv);
    decodedSysexCallback(command,numBytes,decodedBytes);
  }
}

void FirmataExtClass::sendBinarySysex(byte command, byte numBytes, byte *data) {
  byte messageSize = (numBytes<<3)/7;
  if ((numBytes<<8)%7>0) {
    messageSize++;
  }
  Serial.write(START_SYSEX);
  Serial.write(command);
  for (byte i=0;i<messageSize;i++) {
    byte j = i*7;
    byte pos=j>>3;   // 0,0,1,2,3,4,5,6,7,7,8...
    byte shift=j&7;  // 0,7,6,5,4,3,2,1,0,7,6...
    byte out = (data[pos]>>shift) & 0x7F;
    if (shift>1 && pos<(numBytes-1)) {
      out|=(data[pos+1]<<(8-shift))&0x7F;
    }
    Serial.write(out);
  }
  Serial.write(END_SYSEX);
}

byte FirmataExtClass::readBinarySysex(byte numInBytes,byte *inData) {
  byte outBytes=(numInBytes*7)>>3;
  for (byte i=0;i<outBytes;i++) {
    byte j=i<<3;
    byte pos=j/7;
    byte shift=j%7;
    decodedBytes[i]=(inData[pos]>>shift)|((inData[pos+1]<<(7-shift))&0xFF);
  }
  return outBytes;
}

// make one instance for the user to use
FirmataExtClass FirmataExt;

