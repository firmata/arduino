/*
 * FILE: DHT11Firmata.ino
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

#include <Firmata.h>
#include <utility/FirmataExt.h>
#include <utility/FirmataReporting.h>
#include "dht11.h"
#include "DHT11Feature.h"

FirmataReporting reporting;
DHT11Feature dht11;
FirmataExt firmataExt;

void
systemResetCallback()
{
  // no DigitalFirmata or AnalogFirmata configured, DHT11 handles pinMode on each call -> bypass Firmata on reset:
  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_ANALOG(pin)) {
      pinMode(pin, ANALOG);
      digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
    }
    else {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, 0); // sets the output to 0
    }
  }
  firmataExt.reset();
}

void
setup()
{
  firmataExt.addFeature(reporting);

  firmataExt.addFeature(dht11);

  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  Firmata.begin(57600);

  systemResetCallback();  // reset to default config
}

void loop()
{
  while (Firmata.available()) {
    Firmata.processInput();
  }
  if (reporting.elapsed()) {
    dht11.report();
  }
}

