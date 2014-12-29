#include "LedStripFirmata.h"
#include "NeopixelFirmata.h"
#include <Adafruit_NeoPixel.h>

#define TOBYTE(loc) ((argv[loc] + (argv[loc+1] << 7))&0xFF)

NeopixelFirmataImpl::NeopixelFirmataImpl()
{
  pixels = NULL;
}

boolean NeopixelFirmataImpl::handleStripCommand(byte npCommand, byte argc, byte* argv)
{
  byte tmp;

  switch(npCommand) {

    case LED_STRIP_CMD_INIT:
      initialize(
        argv[LED_STRIP_INIT_PARAM_PIN],
        TOBYTE(LED_STRIP_INIT_PARAM_COUNT),
        argv[LED_STRIP_INIT_PARAM_ORDER],
        argv[LED_STRIP_INIT_PARAM_SPEED]
      );
      break;

    case LED_STRIP_CMD_CLEAR:
      pixels->clear();
      break;

    case LED_STRIP_CMD_SHOW:
      pixels->show();
      break;

    case LED_STRIP_CMD_BRIGHTNESS:
      if (argc>2) {
        pixels->setBrightness(TOBYTE(LED_STRIP_BRIGHTNESS_PARAM_VALUE));
      } else {
        reportBrightness();
      }
      break;

    case LED_STRIP_CMD_PIXEL:
      tmp = TOBYTE(LED_STRIP_PIXEL_PARAM_LOCATION);

      if(tmp > pixels->numPixels())
      {
        // Firmata.sendString("NeoPixel: Location out of range");
        return false;
      }

      setPixel(
        tmp,
        TOBYTE(LED_STRIP_PIXEL_PARAM_RED),
        TOBYTE(LED_STRIP_PIXEL_PARAM_GREEN),
        TOBYTE(LED_STRIP_PIXEL_PARAM_BLUE)
      );
      break;

    default:
      // Firmata.sendString("NeoPixel: Unknown Command");
      return false;
  }

  return true;

}

void NeopixelFirmataImpl::reset()
{
  if (pixels != NULL)
  {
    delete pixels;
    pixels = NULL;
  }
}

void NeopixelFirmataImpl::initialize(byte pin, byte count, byte order, byte speed)
{
  reset();

  pixels = new Adafruit_NeoPixel(count, pin, order + speed);
  pixels->begin();
  for(int i=0; i<count; i++) {
    pixels->setPixelColor(i, 0, 0, 255);
    pixels->show();
    delay(50);
  }
  pixels->clear();
  pixels->show();
  reportDone();
}

void NeopixelFirmataImpl::reportBrightness()
{
  byte brightness = pixels->getBrightness();
  Firmata.write(START_SYSEX);
  Firmata.write(LED_STRIP_DATA);
  Firmata.write(LED_STRIP_CMD_BRIGHTNESS);
  Firmata.write(brightness & 0x7F);
  Firmata.write((brightness >> 7) & 0x7F);
  Firmata.write(END_SYSEX);
}

void NeopixelFirmataImpl::setPixel(byte location, byte red, byte green, byte blue)
{
  pixels->setPixelColor(location, red, green, blue);
}
