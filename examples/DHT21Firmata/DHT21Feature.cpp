/*
 * FILE: DHT21Firmata.cpp
 * VERSION: 0.1
 * PURPOSE: DHT21 Temperature & Humidity Sensor feature library for Firmata
 * LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 * to be used with Firmata
 * URL: http://www.firmata.org
 * uses DHT21lib
 * URL: http://arduino.cc/playground/Main/DHT21Lib
 *
 * HISTORY:
 * Norbert Truchsess - Original version
 */

#include "DHT21Feature.h"

void DHT21Feature::handleCapability(byte pin)
{
  if (IS_PIN_DIGITAL(pin)) {
    Firmata.write(DHT21_PIN);
    Firmata.write(14); //2*14 bit
  }
}

boolean DHT21Feature::handlePinMode(byte pin, int mode)
{
  if (mode == DHT21_PIN && IS_PIN_DIGITAL(pin)) {
    return true;
  }
  return false;
}

boolean DHT21Feature::handleSysex(byte command, byte argc, byte *argv)
{
  return false;
}

void DHT21Feature::reset()
{

}

void DHT21Feature::report()
{
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (DHT21_PIN == Firmata.getPinMode(i)) {
      int result = DHT21.read(i);
      switch (result) {
        case DHTLIB_OK:
          Firmata.write(START_SYSEX);
          Firmata.write(RESERVED_COMMAND);
          Firmata.write(DHT21_PIN);
          Firmata.write(i);
          Firmata.write(DHT21.temperature & 0x7F);
          Firmata.write(DHT21.temperature >> 7 & 0x7F);
          Firmata.write(DHT21.humidity & 0x7F);
          Firmata.write(DHT21.humidity >> 7 & 0x7F);
          Firmata.write(END_SYSEX);
          break;
        case DHTLIB_ERROR_CHECKSUM:
          Firmata.sendString("DHT21 error checksum");
          break;
        case DHTLIB_ERROR_TIMEOUT:
          Firmata.sendString("DHT21 error timeout");
          break;
        default:
          Firmata.sendString("DHT21 error unknown");
          break;
      }
    }
  }
}


