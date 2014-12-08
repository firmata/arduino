#ifndef LedStripFirmata_H
#define LedStripFirmata_H

#include <Firmata.h>
#include "FirmataExt.h"

#define LED_STRIP_CMD_QUERY      0x00
#define LED_STRIP_CMD_INIT       0x01
#define LED_STRIP_CMD_PIXEL      0x02
#define LED_STRIP_CMD_CLEAR      0x04
#define LED_STRIP_CMD_SHOW       0x05
#define LED_STRIP_CMD_BRIGHTNESS 0x06
#define LED_STRIP_CMD_DONE       0x07 // Response

#define LED_STRIP_INIT_PARAM_PIN   0x01
#define LED_STRIP_INIT_PARAM_COUNT 0x02
#define LED_STRIP_INIT_PARAM_ORDER 0x04
#define LED_STRIP_INIT_PARAM_SPEED 0x05

#define LED_STRIP_PIXEL_PARAM_LOCATION 0x01
#define LED_STRIP_PIXEL_PARAM_RED      0x03
#define LED_STRIP_PIXEL_PARAM_GREEN    0x05
#define LED_STRIP_PIXEL_PARAM_BLUE     0x07 

#define LED_STRIP_BRIGHTNESS_PARAM_VALUE 0x01

#define LED_STRIP_TYPE_NEOPIXEL 0x00
#define LED_STRIP_TYPE_WS2801   0x01

class LedStripFirmata : public FirmataFeature
{
public:
  LedStripFirmata() {}
  void handleCapability(byte pin);
  boolean handlePinMode(byte pin, int mode);
  boolean handleSysex(byte command, byte argc,  byte* argv);
  virtual void reset() = 0;

protected:
  virtual boolean handleStripCommand(byte command, byte argc, byte* argv) = 0;
  void reportDone();
  void reportType();
  virtual byte ledType() = 0;

  boolean initialized = false;
};

LedStripFirmata* ledStripFirmataFactory();

#endif // LedStripFirmata_H
