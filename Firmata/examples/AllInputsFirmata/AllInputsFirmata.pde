/* Supports as many analog inputs and analog PWM outputs as possible.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>

byte pin;

void setup()
{
  Firmata.setFirmwareVersion(0, 1);
  Firmata.begin(57600);
}

void loop()
{
  while(Firmata.available()) {
    Firmata.processInput();
  }
  for(pin = 0; pin < TOTAL_ANALOG_PINS; pin++) {
    Firmata.sendAnalog(pin, analogRead(pin)); 
  }
  for(pin = 0; pin < TOTAL_DIGITAL_PINS; pin++) {
    Firmata.sendDigital(pin, digitalRead(pin)); 
  }
}


