#include "LedStripFirmata.h"
// Choose one pixel subtype to include
#include "NeopixelFirmata.h"
// #include "Ws2801PixelFirmata.h"  // Not completed yet

void LedStripFirmata::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin))
  {
    Firmata.write(LED_STRIP);
    Firmata.write(1);  // Not sure if this is necessary
  }
}

boolean LedStripFirmata::handlePinMode(byte pin, int mode)
{
  if (mode == LED_STRIP && IS_PIN_DIGITAL(pin))
  {
    return true;
  }
  return false;
}

void LedStripFirmata::reportDone()
{
  Firmata.write(START_SYSEX);
  Firmata.write(LED_STRIP_DATA);
  Firmata.write(LED_STRIP_CMD_DONE);
  Firmata.write(END_SYSEX);
}

void LedStripFirmata::reportType()
{
  Firmata.write(START_SYSEX);
  Firmata.write(LED_STRIP_DATA);
  Firmata.write(LED_STRIP_CMD_QUERY);
  Firmata.write(ledType());
  Firmata.write(END_SYSEX);
}


boolean LedStripFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  if (command != LED_STRIP_DATA)
  {
    return false;
  }

  byte npCommand = argv[0];
  if (!initialized && npCommand != LED_STRIP_CMD_INIT && npCommand != LED_STRIP_CMD_QUERY)
  {
    // Firmata.sendString("Led Strip Not Initialized: Not Initialized");
    return false;
  }

  switch(npCommand) {
    case LED_STRIP_CMD_QUERY:
      reportType();
      return true;

    default:
      return handleStripCommand(npCommand, argc, argv);
  }

  return true;

}


LedStripFirmata* ledStripFirmataFactory()
{
  #ifdef NeopixelFirmata_H
  return new NeopixelFirmataImpl();
  #endif
}
