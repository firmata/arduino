/*
 * FILE: DHT21Firmata.h
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

#ifndef DHT21Feature_h
#define DHT21Feature_h

#include <Firmata.h>
#include <utility/FirmataFeature.h>
#include "dht21.h"

#define DHT21_PIN 0x7e
#define RESERVED_COMMAND 0x00

class DHT21Feature:
  public FirmataFeature
{
  public:
    void handleCapability(byte pin);
    boolean handlePinMode(byte pin, int mode);
    boolean handleSysex(byte command, byte argc, byte *argv);
    void reset();
    void report();
  private:
    dht21 DHT21;
};

#endif

