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
  boolean handleSysex(byte command, byte argc, byte* argv);
  void reset();
  void report();
private:
  dht11 DHT11;
};

#endif

