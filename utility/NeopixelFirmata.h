#ifndef NeopixelFirmata_H
#define NeopixelFirmata_H

#include <Firmata.h>
#include "FirmataExt.h"

#define NEOPIXEL_CMD_INIT       0x01
#define NEOPIXEL_CMD_PIXEL      0x02
#define NEOPIXEL_CMD_CLEAR      0x04
#define NEOPIXEL_CMD_SHOW       0x05
#define NEOPIXEL_CMD_BRIGHTNESS 0x06

#define NEOPIXEL_INIT_PARAM_PIN   0x01
#define NEOPIXEL_INIT_PARAM_COUNT 0x02
#define NEOPIXEL_INIT_PARAM_ORDER 0x04
#define NEOPIXEL_INIT_PARAM_SPEED 0x05

#define NEOPIXEL_PIXEL_PARAM_LOCATION 0x01
#define NEOPIXEL_PIXEL_PARAM_RED      0x03
#define NEOPIXEL_PIXEL_PARAM_GREEN    0x05
#define NEOPIXEL_PIXEL_PARAM_BLUE     0x07 

#define NEOPIXEL_BRIGHTNESS_PARAM_VALUE 0x01

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
