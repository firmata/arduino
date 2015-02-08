/*
 * FILE: DHT11Firmata.cpp
 * VERSION: 0.1
 * PURPOSE: DHT11 Temperature & Humidity Sensor feature library for Firmata
 * LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 * to be used with Firmata
 * URL: http://www.firmata.org
 * uses DHT11lib
 * URL: http://arduino.cc/playground/Main/DHT11Lib
 *
 * HISTORY:
 * Norbert Truchsess - Original version
 */

#include "DHT11Feature.h"

void DHT11Feature::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write(DHT11_PIN);
    Firmata.write(14); //2*14 bit
  }
}

boolean DHT11Feature::handlePinMode(byte pin, int mode)
{
  if (mode == DHT11_PIN && IS_PIN_DIGITAL(pin)) {
    return true;
  }
  return false;
}

boolean DHT11Feature::handleSysex(byte command, byte argc, byte *argv)
{
  return false;
}

void DHT11Feature::reset()
{

}

void DHT11Feature::report()
{
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (DHT11_PIN == Firmata.getPinMode(i)) {
      int result = DHT11.read(i);
      switch (result) {
        case DHTLIB_OK:
          Firmata.write(START_SYSEX);
          Firmata.write(RESERVED_COMMAND);
          Firmata.write(DHT11_PIN);
          Firmata.write(i);
          Firmata.write(DHT11.temperature & 0x7F);
          Firmata.write(DHT11.temperature >> 7 & 0x7F);
          Firmata.write(DHT11.humidity & 0x7F);
          Firmata.write(DHT11.humidity >> 7 & 0x7F);
          Firmata.write(END_SYSEX);
          break;
        case DHTLIB_ERROR_CHECKSUM:
          Firmata.sendString("DHT11 error checksum");
          break;
        case DHTLIB_ERROR_TIMEOUT:
          Firmata.sendString("DHT11 error timeout");
          break;
        default:
          Firmata.sendString("DHT11 error unknown");
          break;
      }
    }
  }
}


