/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please clink on the following link
 * to open the download page in your default browser.
 *
 * http://firmata.org/wiki/Download
 */

/* This firmware supports as many servos as possible using the Servo library 
 * included in Arduino 0017
 *
 * This example code is in the public domain.
 */
 
#include <Servo.h>
#include <Firmata.h>

Servo servos[MAX_SERVOS];
byte servoPinMap[TOTAL_PINS];
byte servoCount = 0;

void analogWriteCallback(byte pin, int value)
{
  if (IS_PIN_DIGITAL(pin)) {
    servos[servoPinMap[pin]].write(value);
  }
}

void setup() 
{
  byte pin;

  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);

  // attach servos from fist digital pin up to max number of
  // servos supported for the board
  for (pin = 0; pin < TOTAL_PINS; pin++) {
    if (IS_PIN_DIGITAL(pin)) {
      if (servoCount < MAX_SERVOS) {
        servos[servoPinMap[pin]].attach(PIN_TO_DIGITAL(pin));
        servoPinMap[pin] = servoCount;
        servoCount++;
      } else {
        Firmata.sendString("Max servos attached");
      }
    }
  }
  
  Firmata.begin(57600);
}

void loop() 
{
  while(Firmata.available())
    Firmata.processInput();
}
