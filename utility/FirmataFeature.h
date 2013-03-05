#ifndef FirmataFeature_h
#define FirmataFeature_h

#include <Firmata.h>

class FirmataFeature
{
public:
  virtual void handleCapability(byte pin) = 0;
  virtual boolean handlePinMode(byte pin, int mode) =0;
  virtual boolean handleSysex(byte command, byte argc, byte* argv) = 0;
  virtual void reset() = 0;
};

#endif
