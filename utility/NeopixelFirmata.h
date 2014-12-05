#ifndef NeopixelFirmata_H
#define NeopixelFirmata_H

#include <Firmata.h>
#include "FirmataExt.h"

class NeopixelFirmata : public FirmataFeature
{
public:
  NeopixelFirmata() {}
  virtual void handleCapability(byte pin) = 0;
  virtual boolean handlePinMode(byte pin, int mode) = 0;
  virtual boolean handleSysex(byte command, byte argc,  byte* argv) = 0;
  virtual void reset() = 0;
};

NeopixelFirmata* neopixelFirmataFactory();

#endif // NeopixelFirmata_H
