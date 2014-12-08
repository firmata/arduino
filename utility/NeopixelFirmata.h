#ifndef NeopixelFirmata_H
#define NeopixelFirmata_H

#include "LedStripFirmata.h"
#include "Adafruit_Neopixel.h"

class NeopixelFirmataImpl : public LedStripFirmata
{
public:
  NeopixelFirmataImpl();
  virtual void reset();

protected:
  byte ledType() { return LED_STRIP_TYPE_NEOPIXEL; }
  virtual boolean handleStripCommand(byte command, byte argc,  byte* argv);
  bool initialized;

private:
  Adafruit_NeoPixel* pixels;
  void initialize(byte pin, byte count, byte order, byte speed);
  void reportBrightness();
  void setPixel(byte position, byte red, byte green, byte blue);
};

#endif // NeopixelFirmata_H

