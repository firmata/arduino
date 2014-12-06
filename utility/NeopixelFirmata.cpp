#include "NeopixelFirmata.h"
#include <Adafruit_NeoPixel.h>

#define TOBYTE(loc) ((argv[loc] + (argv[loc+1] << 7))&0xFF)

class NeopixelFirmataImpl : public NeopixelFirmata
{
public:
  NeopixelFirmataImpl();
  virtual void handleCapability(byte pin);
  virtual boolean handlePinMode(byte pin, int mode);
  virtual boolean handleSysex(byte command, byte argc,  byte* argv);
  virtual void reset();

private:
  Adafruit_NeoPixel* pixels;
  void initialize(byte pin, byte count, byte order, byte speed);
  void reportBrightness();
  void setPixel(byte position, byte red, byte green, byte blue);
  void reportDone();
};

NeopixelFirmataImpl::NeopixelFirmataImpl()
{
  pixels = NULL;
}

void NeopixelFirmataImpl::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin))
  {
    Firmata.write(NEOPIXEL);
    Firmata.write(1);  // Not sure if this is necessary
  }
}

boolean NeopixelFirmataImpl::handlePinMode(byte pin, int mode)
{
  if (mode == NEOPIXEL && IS_PIN_DIGITAL(pin))
  {
    return true;
  }
  return false;
}

boolean NeopixelFirmataImpl::handleSysex(byte command, byte argc, byte* argv)
{
  byte tmp;

  if (command != NEOPIXEL_DATA)
  {
    return false;
  }

  byte npCommand = argv[0];
  if (pixels == NULL && npCommand != NEOPIXEL_CMD_INIT)
  {
    // Firmata.sendString("NeoPixel: Not Initialized");
    return false;
  }

  switch(npCommand) {
    case NEOPIXEL_CMD_INIT:
      initialize(
        argv[NEOPIXEL_INIT_PARAM_PIN],
        TOBYTE(NEOPIXEL_INIT_PARAM_COUNT),
        argv[NEOPIXEL_INIT_PARAM_ORDER],
        argv[NEOPIXEL_INIT_PARAM_SPEED]
      );
      break;

    case NEOPIXEL_CMD_CLEAR:
      pixels->clear();
      break;

    case NEOPIXEL_CMD_SHOW:
      pixels->show();
      break;

    case NEOPIXEL_CMD_BRIGHTNESS:
      if (argc>2) {
        pixels->setBrightness(TOBYTE(NEOPIXEL_BRIGHTNESS_PARAM_VALUE));
      } else {
        reportBrightness();
      }
      break;

    case NEOPIXEL_CMD_PIXEL:
      tmp = TOBYTE(NEOPIXEL_PIXEL_PARAM_LOCATION);

      if(tmp > pixels->numPixels())
      {
        // Firmata.sendString("NeoPixel: Location out of range");
        return false;
      }

      setPixel(
        tmp,
        TOBYTE(NEOPIXEL_PIXEL_PARAM_RED),
        TOBYTE(NEOPIXEL_PIXEL_PARAM_GREEN),
        TOBYTE(NEOPIXEL_PIXEL_PARAM_BLUE)
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
  Firmata.write(NEOPIXEL_DATA);
  Firmata.write(NEOPIXEL_CMD_BRIGHTNESS);
  Firmata.write(brightness & 0x7F);
  Firmata.write((brightness >> 7) & 0x7F);
  Firmata.write(END_SYSEX);
}

void NeopixelFirmataImpl::reportDone()
{
  Firmata.write(START_SYSEX);
  Firmata.write(NEOPIXEL_DATA);
  Firmata.write(NEOPIXEL_CMD_DONE);
  Firmata.write(END_SYSEX);
}

void NeopixelFirmataImpl::setPixel(byte location, byte red, byte green, byte blue)
{
  pixels->setPixelColor(location, red, green, blue);
}

NeopixelFirmata* neopixelFirmataFactory()
{
  return new NeopixelFirmataImpl();
}