/*
 * FILE: DHT11Firmata.h
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

#ifndef DHT11Feature_h
#define DHT11Feature_h

#include <Firmata.h>
#include <utility/FirmataFeature.h>
#include "dht11.h"

#define DHT11_PIN 0x7e
#define RESERVED_COMMAND 0x00

class DHT11Feature:
  public FirmataFeature
{
  public:
    void handleCapability(byte pin);
    boolean handlePinMode(byte pin, int mode);
    boolean handleSysex(byte command, byte argc, byte *argv);
    void reset();
    void report();
  private:
    dht11 DHT11;
};

#endif

