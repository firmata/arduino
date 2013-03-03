#ifndef FirmataCapability_h
#define FirmataCapability_h

#include <Firmata.h>

class FirmataCapability
{
public:
  virtual void handleCapability(byte pin) = 0;
//  virtual ~FirmataCapability();
};

#endif
