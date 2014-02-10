/*
 * To run this test suite, you must first install the ArduinoUnit library
 * to your Arduino/libraries/ directory.
 * You can get ArduinoUnit here: https://github.com/mmurdoch/arduinounit
 * Download version 2.0 or greater.
 */

#include <Wire.h>
#include <Servo.h>
#include <ArduinoUnit.h>
#include "Firmata.h"
#include "utility/EncoderFirmata.h" 

// overide interrupt macro (for unit tests only)
#define IS_PIN_INTERRUPT(p)   ((p) == 2 || (p) == 3 ) 


FakeStream stream;

void setup()
{
  Firmata.begin(stream); // use fake stream
  
  Serial.begin(57600); // must match port configuration
}

void loop()
{
  Test::run();
}

test(handleInterruptPins)
{
  EncoderFirmata encoder;
  assertTrue(encoder.handlePinMode(2, INPUT));
}


test(handleNonInterruptPinS)
{
  EncoderFirmata encoder;
  assertFalse(encoder.handlePinMode(4, INPUT));
}