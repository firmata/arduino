//
//    FILE: dht21.h
// VERSION: 0.4.1
// PURPOSE: DHT21 Temperature & Humidity Sensor library for Arduino
// LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
//
// DATASHEET: http://www.micro4you.com/files/sensor/DHT21.pdf
//
//     URL: http://arduino.cc/playground/Main/DHT21Lib
//
// HISTORY:
// George Hadjikyriacou - Original version
// see dht.cpp file
//

#ifndef dht21_h
#define dht21_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define DHT21LIB_VERSION "0.4.1"

#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2

class dht21
{
  public:
    int read(int pin);
    int humidity;
    int temperature;
};
#endif
//
// END OF FILE
//

