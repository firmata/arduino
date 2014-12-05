#include "NeopixelFirmata.h"

class NeopixelFirmataImpl : public NeopixelFirmata
{
public:
  NeopixelFirmataImpl();
  virtual void handleCapability(byte pin);
  virtual boolean handlePinMode(byte pin, int mode);
  virtual boolean handleSysex(byte command, byte argc,  byte* argv);
  virtual void reset();
};

NeopixelFirmataImpl::NeopixelFirmataImpl()
{
}

void NeopixelFirmataImpl::handleCapability(byte pin)
{
}

boolean NeopixelFirmataImpl::handlePinMode(byte pin, int mode)
{
  return false;
}

boolean NeopixelFirmataImpl::handleSysex(byte command, byte argc, byte* argv)
{
  return false;
}

void NeopixelFirmataImpl::reset()
{
}

NeopixelFirmata* neopixelFirmataFactory()
{
  return new NeopixelFirmataImpl();
}