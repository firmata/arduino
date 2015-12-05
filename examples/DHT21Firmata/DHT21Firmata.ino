/*
 * FILE: DHT21Firmata.ino
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

#include <Firmata.h>
#include <utility/FirmataExt.h>
#include <utility/FirmataReporting.h>
#include "dht21.h"
#include "DHT21Feature.h"

FirmataReporting reporting;
DHT21Feature dht21;
FirmataExt firmataExt;

void
systemResetCallback()
{
  // no DigitalFirmata or AnalogFirmata configured, DHT21 handles pinMode on each call -> bypass Firmata on reset:
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

  firmataExt.addFeature(dht21);

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
    dht21.report();
  }
}

