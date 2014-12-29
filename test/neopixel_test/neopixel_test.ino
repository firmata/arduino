#include <Firmata.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#include <utility/FirmataExt.h>
FirmataExt firmataExt;

#include "utility/neopixelFirmata.h"
NeopixelFirmata* neopixelFirmata = neopixelFirmataFactory();

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void systemResetCallback()
{
  // initialize a defalt state

  // pins with analog capability default to analog input
  // otherwise, pins default to digital output
  for (byte i=0; i < TOTAL_PINS; i++) {
  }
  
  firmataExt.reset();
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void setup() 
{
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  firmataExt.addFeature(*neopixelFirmata);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  // start up the default Firmata using Serial interface:
  Firmata.begin(57600);
  systemResetCallback();  // reset to default config
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() 
{

  while(Firmata.available()) {
    Firmata.processInput();
  }
  
}

